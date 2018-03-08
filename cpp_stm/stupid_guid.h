#ifndef STUPID_GUID_H
#define STUPID_GUID_H

#include <string>
#include <random>

namespace utils
{

using GUID = std::string;

// Don't have time to make it properly.
template <typename Dice>
GUID newGUID(Dice& dice)
{
    std::string s;
    for (auto i = 0; i < 32; ++i)
    {
        char c = dice();
        s += c <= 10 ? '0' + (c - 1) : 'a' + (c - 11);
    }
    return s;
}

} // namespace utils

#endif // STUPID_GUID_H
