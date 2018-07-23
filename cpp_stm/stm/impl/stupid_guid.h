#ifndef STUPID_GUID_H
#define STUPID_GUID_H

#include <string>
#include <random>
#include <functional>

namespace utils
{

using GUID = std::string;
using Dice = std::_Bind<std::uniform_int_distribution<int>(
    std::linear_congruential_engine<long unsigned int, 16807, 0, 2147483647>)>;

// Don't have time to make it properly.
GUID newGUID(Dice& dice);

} // namespace utils

#endif // STUPID_GUID_H
