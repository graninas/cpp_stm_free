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

template <typename T, typename Next>
struct Pure
{
    T ret;
};

template <typename AlgebraMethod, typename Ret>
struct Free
{
    AlgebraMethod method;
};

template <typename Ret>
Free<Pure<Ret, fp::Unit>, Ret>
    pureFree(const Ret& ret)
    {
        auto f = Free<Pure<Ret, fp::Unit>, Ret>();
        f.method.ret = ret;
        return f;
    }

// newTVar :: a -> Free STMAlgebra (TVar a)
template <typename T>
Free<NewTVar<T,
             Free<Pure<T, fp::Unit>, T>>,
    fp::Unit>
    newTVar(const T& val)
{
    auto f = Free<NewTVar<T, Free<Pure<T, fp::Unit>, T>>, fp::Unit>();
    f.method.val  = val;
//    f.method.next = [](const T& ret) { return pureFree(ret); };
    f.method.next = [](auto ret) { return pureFree(ret); };
    return f;
}

// writeTVar :: TVar a -> a -> Free STMAlgebra Unit
template <typename T>
Free<WriteTVar<T, fp::Unit>, fp::Unit>
    writeTVar(const TVar<T>& tvar, const T& val)
{
    auto f = Free<WriteTVar<T, fp::Unit>, fp::Unit>();
    f.method.tvar = tvar;
    f.method.val  = val;
    f.method.next = fp::unit;
    return f;
}

// readTVar :: TVar a -> Free STMAlgebra a
// Wrong type (see newTVar)
template <typename T>
Free<ReadTVar<T, fp::Unit>, fp::Unit>
    readTVar(const TVar<T>& tvar)
{
    auto f = Free<ReadTVar<T, fp::Unit>, fp::Unit>();
    f.method.tvar = tvar;
    f.method.next = fp::id;
    return f;
}


template <typename T>
T unFree(const Free<Pure<T, fp::Unit>, T>& pureVal)
{
    return pureVal.method.ret;
}


// bind :: m a -> (a -> m b) -> m b
template <typename T, typename Method, typename Ret>
void
    bindFree(const Free<Pure<T, fp::Unit>, T>& pureVal,
//             const std::function<
//                Free<Method, Ret>
//                (T)>& cont
             const std::function<T(T)>& dummy
             )
{
    auto val = pureVal.method.ret;
    auto result = dummy(val);
}



} // namespace stm

#endif // STM_FREE_H
