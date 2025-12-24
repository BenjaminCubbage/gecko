#pragma once
#include <limits>
#include <string_view>

namespace Gecko::API::Http
{
    class ParseValue
    {
    public:
        // Safely parse a string comprised of digits to an integral, 
        // up to a value of std::numeric_limits<T>::max()
        // Leading hyphen / signage not supported
        // Leading zeroes are OK but parsing will fail if the string's size
        // is greater than std::numeric_limits<T>::digits10 + 1
        // Example:
        //      TryParseIntegral<uint8_t>("255", &result) -> true
        //      TryParseIntegral<uint8_t>("256", &result) -> false (output argument untouched)
        template<class T>
            requires std::is_integral_v<T>
        static bool TryParseIntegral(std::string_view str, T* outResult)
        {
            // note(ben): numeric limit is w/out change
            constexpr size_t maxDigits = std::numeric_limits<T>::digits10 + 1;
            constexpr size_t maxVal    = std::numeric_limits<T>::max();

            if (!str.size() || str.size() > maxDigits)
                return false;

            T result{};

            for (int i = 0; i < str.size() - 1; ++i)
            {
                if (!IsDigit(str[i]))
                    return false;

                result = result * 10 + DigitValue(str[i]);
            }

            // note(ben): This is for the last digit (overflow protection)
            if (!IsDigit(str.back()))
                return false;

            if (result > maxVal / 10)
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