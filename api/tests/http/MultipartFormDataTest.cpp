#include "gtest/gtest.h"
#include "gecko/http/MultipartFormData.h"
#include "httplib.h"
#include <cstdint>
#include <string>
#include <vector>

namespace Http = ::Gecko::API::Http;

namespace Gecko::API::Test
{
    struct Part {
        std::string name;
        std::vector<std::string> chunks; // ordered
    };

    static httplib::ContentReader MakeReader(const std::vector<Part>& parts)
    {
        // Raw-body reader (unused by FromContentReader) — just accept and return true.
        httplib::ContentReader::Reader raw_reader =
            [](httplib::ContentReceiver) -> bool { return true; };

        // Multipart reader: receives the two callbacks (headerFn, receiverFn) and replays scripted parts.
        httplib::ContentReader::FormDataReader multipart_reader =
            [parts](httplib::FormDataHeader headerFn, httplib::ContentReceiver receiverFn) -> bool
        {
            for (const auto& p : parts)
            {
                httplib::FormData item{};
                item.name = p.name;

                if (!headerFn(item))
                    return false;

                for (const auto& c : p.chunks)
                    if (!receiverFn(c.data(), c.size()))
                        return false;
            }
            return true;
        };

        return httplib::ContentReader{ std::move(raw_reader), std::move(multipart_reader) };
    }

    static std::string Bytes(std::initializer_list<uint8_t> b)
    {
        return std::string(reinterpret_cast<const char*>(b.begin()), b.size());
    }

    TEST(MultipartFormData, ParsesStringFieldSingleChunk)
    {
        std::vector<Http::MultipartFormData::FieldFinder> finders{
            { "title", Http::MultipartFormData::FieldFinder::FieldType::String, 16 },
        };

        auto reader = MakeReader({
            { "title", { "hello" } },
        });

        auto mfd = Http::MultipartFormData::FromContentReader(reader, finders);

        std::string* title = nullptr;
        ASSERT_TRUE(mfd.Find("title", &title));
        ASSERT_NE(title, nullptr);
        EXPECT_EQ(*title, "hello");
    }

    TEST(MultipartFormData, ConcatenatesStringFieldMultipleChunks)
    {
        std::vector<Http::MultipartFormData::FieldFinder> finders{
            { "title", Http::MultipartFormData::FieldFinder::FieldType::String, 16 },
        };

        auto reader = MakeReader({
            { "title", { "hel", "lo", " ", "world" } },
        });

        auto mfd = Http::MultipartFormData::FromContentReader(reader, finders);

        std::string* title = nullptr;
        ASSERT_TRUE(mfd.Find("title", &title));
        ASSERT_NE(title, nullptr);
        EXPECT_EQ(*title, "hello world");
    }

    TEST(MultipartFormData, ParsesVectorFieldAndPreservesBinaryData)
    {
        std::vector<Http::MultipartFormData::FieldFinder> finders{
            { "blob", Http::MultipartFormData::FieldFinder::FieldType::Vector, 8 },
        };

        // note(ben): Includes a 0x00 byte to prove binary is preserved
        auto reader = MakeReader({
            { "blob", { Bytes({0xDE, 0xAD}), Bytes({0x00, 0xBE, 0xEF}) } },
        });

        auto mfd = Http::MultipartFormData::FromContentReader(reader, finders);

        std::vector<uint8_t>* blob = nullptr;
        ASSERT_TRUE(mfd.Find("blob", &blob));
        ASSERT_NE(blob, nullptr);

        const std::vector<uint8_t> expected{ 0xDE, 0xAD, 0x00, 0xBE, 0xEF };
        EXPECT_EQ(*blob, expected);
    }

    TEST(MultipartFormData, IgnoresUnlistedFields)
    {
        std::vector<Http::MultipartFormData::FieldFinder> finders{
            { "kept", Http::MultipartFormData::FieldFinder::FieldType::String, 8 },
        };

        auto reader = MakeReader({
            { "ignored", { "nope" } },
            { "kept",    { "yes"  } },
        });

        auto mfd = Http::MultipartFormData::FromContentReader(reader, finders);

        std::string* kept = nullptr;
        ASSERT_TRUE(mfd.Find("kept", &kept));
        ASSERT_NE(kept, nullptr);
        EXPECT_EQ(*kept, "yes");

        std::string* ignored = reinterpret_cast<std::string*>(0x1);
        EXPECT_FALSE(mfd.Find("ignored", &ignored));
        EXPECT_EQ(ignored, nullptr);
    }

    TEST(MultipartFormData, DuplicateFieldName_LastOneWins)
    {
        std::vector<Http::MultipartFormData::FieldFinder> finders{
            { "title", Http::MultipartFormData::FieldFinder::FieldType::String, 8 },
        };

        auto reader = MakeReader({
            { "title", { "first" } },
            { "title", { "second" } },
        });

        auto mfd = Http::MultipartFormData::FromContentReader(reader, finders);

        std::string* title = nullptr;
        ASSERT_TRUE(mfd.Find("title", &title));
        ASSERT_NE(title, nullptr);
        EXPECT_EQ(*title, "second");
    }

    TEST(MultipartFormData, FindMissingFieldNullsOutPointerAndReturnsFalse)
    {
        auto reader = MakeReader({}); // no parts

        std::vector<Http::MultipartFormData::FieldFinder> finders{
            { "title", Http::MultipartFormData::FieldFinder::FieldType::String, 8 },
            { "blob",  Http::MultipartFormData::FieldFinder::FieldType::Vector, 8 },
        };

        auto mfd = Http::MultipartFormData::FromContentReader(reader, finders);

        std::string* s = reinterpret_cast<std::string*>(0x1);
        EXPECT_FALSE(mfd.Find("missing", &s));
        EXPECT_EQ(s, nullptr);

        std::vector<uint8_t>* v = reinterpret_cast<std::vector<uint8_t>*>(0x1);
        EXPECT_FALSE(mfd.Find("missing", &v));
        EXPECT_EQ(v, nullptr);
    }
}