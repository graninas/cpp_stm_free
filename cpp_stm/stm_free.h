#ifndef STM_FREE_H
#define STM_FREE_H

#include <functional>

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

template <typename T, typename Next>
struct NewTVar
{
    using Cont = Next;

    T val;
    std::function<Next(TVar<T>)> next;

//    template<typename ToType>
//    Free<NewTVar<ToType, Next>>
//        map(const Fun(T, ToType)& f) const
//        {
//            Free<NewTVar<ToType, Next>> fmapped;
//            fmapped.method.val  = f(val);
//            fmapped.method.next = next;
//            return fmapped;
//        }
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

    template<typename ToType>
    Free<Pure<ToType, fp::Unit>>
        map(const Fun(T, ToType)& f) const
        {
            return pureFree(f(ret));
        }

//    Pure<T, Next>& operator= (const Pure<T, Next>& other)
//        {
//            ret = other.ret;
//            return *this;
//        }
};

template <typename AlgebraMethod>
struct Free
{
    AlgebraMethod method;

//    Free<AlgebraMethod>& operator= (const Free<AlgebraMethod>& other)
//        {
//            method = other.method;
//            return *this;
//        }

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

template < typename T1
         , typename T2
         , typename Next
         , template<typename, typename> class Method>
Free<Method<T2, Next>>
    mapFree(const Free<Method<T1, Next>>& m,
            const std::function<T2(T1)>& f)
{
    return m.method.map(f);
}

// for test
template <typename T, template<typename, typename> class Method, typename Ret1>
void passFree(const Free<Method<T, Ret1>>&)
{
}


} // namespace stm

#endif // STM_FREE_H

