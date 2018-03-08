#include "stupid_guid.h"

namespace utils {

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

}
