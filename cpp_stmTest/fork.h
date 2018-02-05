#ifndef FORK_H
#define FORK_H

#include <string>
#include "common.h"

namespace sample
{

enum class ForkState: int
{
    Free,
    Taken
};

struct Fork
{
    std::string name;
    ForkState state;
};

} // namespace sample


#endif // FORK_H
