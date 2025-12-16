#pragma once
#include <limits>
#include <string_view>

namespace Gecko::API::Http
{
    class ParseValue
    {
    public:
        template<class T>
            requires std::is_integral_v<T> && std::is_unsigned_v<T>
        static bool TryParseUIntegral(std::string_view str, T* outResult)
        {
            // note(ben): numeric limit is w/out change
            constexpr size_t maxDigits   = std::numeric_limits<T>::digits10 + 1;
            constexpr size_t maxVal      = std::numeric_limits<T>::max();
            constexpr size_t maxValDiv10 = maxVal / 10;

            if (!str.size() || str.size() > maxDigits)
                return false;

            T result{};

            for (int i = 0; i < str.size() - 1; ++i)
            {
                if (!IsDigit(str[i]))
                    return false;

                result = result * 10 + DigitValue(str[i]);
            }

            if (!IsDigit(str.back()))
                return false;

            if (result > maxValDiv10)
                return false;

            result = result * 10;

            T lastValue = DigitValue(str.back());

            if (lastValue > maxVal - result)
                return false;

            result += lastValue;

            *outResult = result;
            return true;
        }

    private:
        static inline bool IsDigit(char c)
        {
            return c >= '0' && c <= '9';
        }

        static inline unsigned DigitValue(char c)
        {
            return c - '0';
        }
    };
}