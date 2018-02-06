#ifndef STM_FREE_H
#define STM_FREE_H

#include <functional>

//#include <unit.h>
#include <identity.h>

#include "tvar.h"
//#include "stm_free_stack.h"

namespace fp
{
struct Unit
{

};

const Unit unit;

}

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


//template <typename Pure>
//struct STMPure<Pure>
//{
//    Pure val;
//};


//template <typename T>
//STMFree
//    pure(const T& val)
//{
//    return STMFree<T>(val);
//}


//// readTVar :: TVar a -> STMFree a
//template <typename T>
//STMFree
//    readTVar(const TVar<T>& tvar)
//{
//    auto m = ReadTVar<T, fp::Identity>();
//    m.tvar = tvar;
//    m.next = ???;
//    return m;
//}


struct FreeC
{
};

struct PureC
{
};

template <typename FreeC, typename AlgebraMethod, typename Ret>
struct Free
{
    AlgebraMethod method;
};

template <typename PureC, typename AlgebraMethod, typename Ret>
struct Free
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


} // namespace stm

#endif // STM_FREE_H
