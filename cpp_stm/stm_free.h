#ifndef STM_FREE_H
#define STM_FREE_H

#include <functional>

#include <unit.h>
#include <identity.h>

#include "tvar.h"
#include "stm_free_stack.h"

namespace stm
{

template <typename T, typename Next>
struct ReadTVar
{
    TVar<T> tvar;
    Next next;
};

template <typename T, typename Next>
struct WriteTVar
{
    TVar<T> tvar;
    T val;
    Next next;
};

STMFree readTVar(const T& tvar)
{
    auto m = ReadTVar<T, Unit>();
    m.tvar = tvar;
    return m;
}


template <typename T>
STMFree
    writeTVar(const TVar<T>& tvar, const T& val)
{
    auto m = WriteTVar<T>();
    m.tvar = tvar;
    m.val  = val;
    return m;
}


} // namespace stm

#endif // STM_FREE_H
