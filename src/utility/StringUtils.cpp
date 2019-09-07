
#include "StringUtils.hpp"

namespace
{
    const char chars[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    int stringLength = sizeof(chars) - 1;

    char getRandomChar() { return chars[rand() % stringLength]; }  // NOLINT
}

namespace kvs
{
    std::string generateString(size_t length)
    {
        static auto unused = [] {
            srand(static_cast<unsigned int>(time(nullptr)));
            return 0;
        }();
        (void)unused;

        std::string res;
        res.reserve(length);
        for (size_t i = 0; i < length; i++) {
            res += ::getRandomChar();
        }
        return res;
    }
}
