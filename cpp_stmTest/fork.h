#ifndef FORK_H
#define FORK_H

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
    ForkState state;
};

} // namespace sample


#endif // FORK_H
