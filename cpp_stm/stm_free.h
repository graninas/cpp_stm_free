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

#define FreePureT Free<Pure<T, fp::Unit>, T>

template <typename T>
FreePureT pureFree(const T& ret)
{
    auto f = FreePureT();
    f.method.ret = ret;
    return f;
}

// newTVar :: a -> Free STMAlgebra (TVar a)
template <typename T>
Free<NewTVar<T, FreePureT>, fp::Unit>
    newTVar(const T& val)
{
    auto f = Free<NewTVar<T, FreePureT>, fp::Unit>();
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
T unFree(const FreePureT& pureVal)
{
    return pureVal.method.ret;
}

// bind :: m a -> (a -> m b) -> m b
template <typename T>
FreePureT bindFree(const FreePureT& pureVal,
                   const std::function<FreePureT(T)>& f)
{
    auto val = pureVal.method.ret;
    return f(val);
}

template <typename T>
FreePureT bindFree(const FreePureT& pureVal,
                   const std::function<FreePureT(T)>& f)
{
    auto val = pureVal.method.ret;
    return f(val);
}

} // namespace stm

#endif // STM_FREE_H
