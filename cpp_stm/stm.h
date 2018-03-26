#ifndef STM_H
#define STM_H

#include "context.h"
#include "stm_runtime.h"

#include "stm_free.h"
#include "stm_bind.h"

namespace stm
{

template <typename A>
A atomically(Context& context, const STML<A>& stml)
{
    return runSTM(context, stml);
}

template <typename A>
STML<TVar<A>>
    newTVar(const A& val)
{
    NewTVar<A, STML<TVar<A>>> n;
    n.val = val;
    n.next = [](const TVar<A>& tvar) {
        return pureF(tvar);
    };
    return wrapT(n);
}

template <typename A>
STML<A>
    readTVar(const TVar<A>& tvar)
{
    ReadTVar<A, STML<A>> n;
    n.tvar = tvar;
    n.next = [](const A& val) {
        return pureF(val);
    };
    return wrapT(n);
}


template <typename A>
STML<fp::Unit>
    writeTVar(const TVar<A>& tvar, const A& val)
{
    WriteTVar<A, STML<fp::Unit>> n;
    n.tvar = tvar;
    n.val  = val;
    n.next = [](const fp::Unit& unit) {
        return pureF(unit);
    };
    return wrapT(n);
}

} // namespace stm

#endif // STM_H
