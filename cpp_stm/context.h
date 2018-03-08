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

using UStamp = utils::GUID;

struct TVarHandle
{
    UStamp ustamp;
    std::any data;
};

using TVars = std::map<TVarId, TVarHandle>;

class Context
{
private:

    TVars _tvars;

    std::default_random_engine _generator;
    std::uniform_int_distribution<int> _distribution;
    utils::Dice _dice;

    std::mutex _lock;

public:
    Context();

    TVarId newGUID();
};

class AtomicRuntime
{
private:
    Context& _context;

    UStamp _ustamp;
    TVars _localTVars;

public:
    AtomicRuntime(Context& context, UStamp ustamp);

    TVarId newGUID();

    UStamp getUStamp() const;
    void addTVarHandle(const TVarId& tvarId, const TVarHandle& tvarHandle);

};



} // namespace stm

#endif // CONTEXT_H
