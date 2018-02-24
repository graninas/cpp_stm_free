#ifndef STM_FREE_H
#define STM_FREE_H

#include <functional>
#include <any>

#include <unit.h>
#include <identity.h>

#include "tvar.h"
//#include "stm_free_stack.h"

namespace stm
{

#define FreePureT Free<Pure<T, fp::Unit>>
#define Fun(FromT, ToT) std::function<ToT(FromT)>

// forward declarations
template <typename AlgebraMethod>
struct Free;

template <typename T, typename Next>
struct Pure;

template <typename T>
FreePureT pureFree(const T& ret);

// STM Free

template <typename Next>
struct NewTVar
{
    using Cont = Next;

    std::any val;
    std::function<Next(TVar<T>)> next;
};

template <typename T, typename Next>
struct ReadTVar
{
    using Cont = Next;

    TVar<T> tvar;
    std::function<Next(T)> next;
};

template <typename T, typename Next>
struct WriteTVar
{
    using Cont = Next;

    TVar<T> tvar;
    T val;
    Next next;
};

template <typename T, typename Next>
struct Pure
{  
    using Cont = Next;

    T ret;
};

template <typename AlgebraMethod>
struct Free
{
    AlgebraMethod method;
};

template <typename T>
FreePureT pureFree(const T& ret)
{
    auto f = FreePureT();
    f.method.ret = ret;
    return f;
}

// newTVar :: a -> Free STMAlgebra (TVar a)
template <typename T, typename Next>
Free<NewTVar<T, Next>>
    newTVar(const T& val)
{
    auto f = Free<NewTVar<T, Next>>();
    f.method.val  = val;
    f.method.next = fp::id;
    return f;
}

/*
data F a =
    ForInt Int (String -> a)

forInt :: Int -> Free F String
forInt i = Free (ForInt i return)

    */


// writeTVar :: TVar a -> a -> Free STMAlgebra Unit
template <typename T>
Free<WriteTVar<T, fp::Unit>>
    writeTVar(const TVar<T>& tvar, const T& val)
{
    auto f = Free<WriteTVar<T, fp::Unit>>();
    f.method.tvar = tvar;
    f.method.val  = val;
    f.method.next = fp::unit;
    return f;
}

// readTVar :: TVar a -> Free STMAlgebra a
// Wrong type (see newTVar)
template <typename T, typename Next>
Free<ReadTVar<T, Next>>
    readTVar(const TVar<T>& tvar)
{
    auto f = Free<ReadTVar<T, Next>>();
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

template < typename T
         , typename Next1
         , typename Next2
         , template<typename, typename> class Method>
Free<Method<T, Next2>>
    mapFree(const std::function<Next2(Next1)>& f,
            const Free<Method<T, Next1>>& m)
{
    return m.method.map(f);
}

//template <typename T1, typename T2>
//Free<Pure<T2, fp::Unit>>
//    mapFree(const std::function<T2(T1)>& f,
//            const Free<Pure<T1, fp::Unit>>& m)
//{
//    return m.method.map(f);
//}

// for test
template <typename T, template<typename, typename> class Method, typename Ret1>
void passFree(const Free<Method<T, Ret1>>&)
{
}


} // namespace stm

#endif // STM_FREE_H

