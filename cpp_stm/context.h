#ifndef CONTEXT_H
#define CONTEXT_H

#include <map>
#include <any>
#include <mutex>
#include <random>

#include "stupid_guid.h"
#include "tvar.h"

namespace stm
{

struct TVarHandle
{
    utils::GUID ustamp;
    std::any data;
};


class Context
{
private:

    std::map<TVarId, TVarHandle> _tvars;
    std::mutex _lock;

//    std::default_random_engine _generator;
//    std::uniform_int_distribution<int> _distribution;
//    auto dice = std::bind ( distribution, generator );

public:
    Context();
};



} // namespace stm

#endif // CONTEXT_H
