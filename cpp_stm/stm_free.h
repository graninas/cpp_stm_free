#ifndef STM_FREE_H
#define STM_FREE_H

#include <functional>

#include <unit.h>
#include <identity.h>

#include "tvar.h"
//#include "stm_free_stack.h"

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

struct FreeC {};
struct PureC {};

template <typename FreeMethod, typename AlgebraMethod, typename Ret>
struct Free
{
};

template <typename AlgebraMethod, typename Ret>
struct Free<FreeC, AlgebraMethod, Ret>
{
    AlgebraMethod method;
};

template <typename AlgebraMethod, typename Ret>
struct Free<PureC, AlgebraMethod, Ret>
{
    Ret ret;
};

// writeTVar :: TVar a -> a -> Free STMAlgebra Unit
template <typename T>
Free<FreeC, WriteTVar<T, fp::Unit>, fp::Unit>
    writeTVar(const TVar<T>& tvar, const T& val)
{
    auto m = WriteTVar<T, fp::Unit>();
    m.tvar = tvar;
    m.val  = val;
    m.next = fp::unit;

    auto f = Free<FreeC, WriteTVar<T, fp::Unit>, fp::Unit>();
    f.method = f;
    return f;
}

// readTVar :: TVar a -> Free STMAlgebra a
template <typename T>
Free<FreeC, ReadTVar<T, fp::Unit>, fp::Unit>
    readTVar(const TVar<T>& tvar)
{
    auto m = ReadTVar<T, fp::Unit>();
    m.tvar = tvar;
    m.next = fp::unit;

    auto f = Free<FreeC, ReadTVar<T, fp::Unit>, fp::Unit>();
    f.method = f;
    return f;
}

} // namespace stm

#endif // STM_FREE_H
