#include "gtest/gtest.h"
#include "gecko/middleware/HasMultipartFormDataField.h"
#include "gecko/http/MultipartFormData.h"
#include "httplib.h"
#include "json/json.h"
#include <cstdint>
#include <string>
#include <vector>

namespace Http = ::Gecko::API::Http;
using ::Gecko::API::Middleware::HasMultipartFormDataField;

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

    static void ExpectErrorJson(const httplib::Response& res)
    {
        ASSERT_FALSE(res.body.empty());
        Json::Value  j{};
        Json::Reader r{};
        ASSERT_TRUE(r.parse(res.body, j));
        ASSERT_TRUE(j.isMember("error"));
    }

    TEST(HasMultipartFormDataField_String, MissingFieldReturnsFalseAndSetsMissingField)
    {
        httplib::Request  req{};
        httplib::Response res{};
        res.status = httplib::StatusCode::OK_200;

        // Build formData containing some other field.
        std::vector<Http::MultipartFormData::FieldFinder> finders{
            { "other", Http::MultipartFormData::FieldFinder::FieldType::String, 8 },
        };
        auto reader = MakeReader({ { "other", { "x" } } });
        Http::MultipartFormData formData = Http::MultipartFormData::FromContentReader(reader, finders);

        HasMultipartFormDataField<std::string> mw{ "title" };

        std::string* out = reinterpret_cast<std::string*>(0x1);
        const bool ok = mw(req, res, formData, &out);

        ASSERT_FALSE(ok);
        EXPECT_EQ(out, nullptr);

        EXPECT_EQ(res.status, httplib::StatusCode::BadRequest_400); // adjust if your MissingField differs
        ExpectErrorJson(res);
    }

    TEST(HasMultipartFormDataField_String, PresentFieldReturnsTrueSetsOutAndLeavesResponseUntouched)
    {
        httplib::Request  req{};
        httplib::Response res{};
        res.status = httplib::StatusCode::OK_200;
        res.body.clear();

        std::vector<Http::MultipartFormData::FieldFinder> finders{
            { "title", Http::MultipartFormData::FieldFinder::FieldType::String, 8 },
        };
        auto reader = MakeReader({ { "title", { "hello" } } });
        Http::MultipartFormData formData = Http::MultipartFormData::FromContentReader(reader, finders);

        HasMultipartFormDataField<std::string> mw{ "title" };

        std::string* out = nullptr;
        const bool ok = mw(req, res, formData, &out);

        ASSERT_TRUE(ok);
        ASSERT_NE(out, nullptr);
        EXPECT_EQ(*out, "hello");

        EXPECT_EQ(res.status, httplib::StatusCode::OK_200);
        EXPECT_TRUE(res.body.empty());
    }

    TEST(HasMultipartFormDataField_Vector, MissingFieldReturnsFalseAndSetsMissingField)
    {
        httplib::Request  req{};
        httplib::Response res{};
        res.status = httplib::StatusCode::OK_200;

        std::vector<Http::MultipartFormData::FieldFinder> finders{
            { "other", Http::MultipartFormData::FieldFinder::FieldType::Vector, 8 },
        };
        auto reader = MakeReader({ { "other", { Bytes({0xAA}) } } });
        Http::MultipartFormData formData = Http::MultipartFormData::FromContentReader(reader, finders);

        HasMultipartFormDataField<std::vector<uint8_t>> mw{ "blob" };

        std::vector<uint8_t>* out = reinterpret_cast<std::vector<uint8_t>*>(0x1);
        const bool ok = mw(req, res, formData, &out);

        ASSERT_FALSE(ok);
        EXPECT_EQ(out, nullptr);

        EXPECT_EQ(res.status, httplib::StatusCode::BadRequest_400); // adjust if your MissingField differs
        ExpectErrorJson(res);
    }

    TEST(HasMultipartFormDataField_Vector, PresentFieldReturnsTrueSetsOutAndLeavesResponseUntouched)
    {
        httplib::Request  req{};
        httplib::Response res{};
        res.status = httplib::StatusCode::OK_200;
        res.body.clear();

        std::vector<Http::MultipartFormData::FieldFinder> finders{
            { "blob", Http::MultipartFormData::FieldFinder::FieldType::Vector, 8 },
        };
        auto reader = MakeReader({ { "blob", { Bytes({0xDE, 0xAD}), Bytes({0xBE, 0xEF}) } } });
        Http::MultipartFormData formData = Http::MultipartFormData::FromContentReader(reader, finders);

        HasMultipartFormDataField<std::vector<uint8_t>> mw{ "blob" };

        std::vector<uint8_t>* out = nullptr;
        const bool ok = mw(req, res, formData, &out);

        ASSERT_TRUE(ok);
        ASSERT_NE(out, nullptr);

        const std::vector<uint8_t> expected{ 0xDE, 0xAD, 0xBE, 0xEF };
        EXPECT_EQ(*out, expected);

        EXPECT_EQ(res.status, httplib::StatusCode::OK_200);
        EXPECT_TRUE(res.body.empty());
    }
}