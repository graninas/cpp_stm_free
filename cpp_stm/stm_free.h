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
struct NewTVar
{
    T val;
    std::function<Next(T)> next;
};

template <typename T, typename Next>
struct ReadTVar
{
    TVar<T> tvar;
    std::function<Next(T)> next;
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

template <typename AlgebraMethod, typename Ret>
    Free<PureC, AlgebraMethod, Ret>
    pureFree(const Ret& ret)
    {
        auto f = Free<PureC, AlgebraMethod, Ret>();
        f.ret = ret;
        return f;
    }

// newTVar :: a -> Free STMAlgebra (TVar a)
template <typename T>
Free<FreeC, NewTVar<T, T>, fp::Unit>
    newTVar(const T& val)
{
    auto f = Free<FreeC, NewTVar<T, T>, fp::Unit>();
    f.method.val  = val;
    f.method.next = [](const T& ret) { return pureFree(ret); };
//    f.method.next = [](const T& ret)
//        {
//            auto ff = Free<PureC, NewTVar<T, T>, T>();
//            ff.ret = ret;
//            return ff;
//        };
    return f;
}

// writeTVar :: TVar a -> a -> Free STMAlgebra Unit
template <typename T>
Free<FreeC, WriteTVar<T, fp::Unit>, fp::Unit>
    writeTVar(const TVar<T>& tvar, const T& val)
{
    auto f = Free<FreeC, WriteTVar<T, fp::Unit>, fp::Unit>();
    f.method.tvar = tvar;
    f.method.val  = val;
    f.method.next = fp::unit;
    return f;
}

// readTVar :: TVar a -> Free STMAlgebra a
template <typename T>
Free<FreeC, ReadTVar<T, T>, fp::Unit>
    readTVar(const TVar<T>& tvar)
{
    auto f = Free<FreeC, ReadTVar<T, T>, fp::Unit>();
    f.method.tvar = tvar;
    f.method.next = fp::id;
    return f;
}

//// bind :: m a -> (a -> m b) -> m b
//// bind :: NewTVarFree<T> -> (T -> ReadTVarFree<T>) -> NewReadTVarFree<T>
//template <typename T>
//Free<FreeC, NewTVar<T, fp::Unit>, fp::Unit>
//    bindFree(const TVar<T>& tvar,
//             const std::function<(T)>& cont
//             )
//{

//    Free<FreeC, NewTVar<T,
//                        Free<FreeC, ReadTVar<T, fp::Unit>, fp::Unit>
//                       >, fp::Unit>
//}

} // namespace stm

#endif // STM_FREE_H
