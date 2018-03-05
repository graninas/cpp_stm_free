#ifndef STM_FREE_H
#define STM_FREE_H

#include <functional>
#include <any>
#include <variant>

#include <unit.h>
#include <identity.h>

#include "free.h"

#include "tvar.h"
//#include "stm_free_stack.h"

namespace stm
{

// STM Free

template <typename T, typename Next>
struct NewTVarC
{
    T val;
    std::function<Next(TVar<T>)> next;
};

template <typename Next>
struct NewTVarW
{
    using Method = NewTVarC<std::any, Next>;
    Method method;
};

template <typename T, typename Next>
struct ReadTVarC
{
    TVar<T> tvar;
    std::function<Next(T)> next;
};

template <typename Next>
struct ReadTVarW
{
    using Method = ReadTVarC<std::any, Next>;
    Method method;
};

template <typename T, typename Next>
struct WriteTVarC
{
    TVar<T> tvar;
    T val;
    Next next;
};

template <typename Next>
struct WriteTVarW
{
    using Method = WriteTVarC<std::any, Next>;
    Method method;
};

template <class Next>
  using STMF = std::variant<NewTVarW<Next>, ReadTVarW<Next>, WriteTVarW<Next>>;

template <class Next>
    using STML = fp::FreeT<STMF, Next>;


// newTVar :: a -> Free Algebra (TVar a)
template <typename T>
NewTVarC<T, TVar<T>>
    newTVarC(const T& val)
{
    auto f = NewTVarC<T, TVar<T>>();
    f.val = val;
    f.next = fp::id;
    return f;
}


// writeTVar :: TVar a -> a -> Free Algebra Unit
template <typename T>
WriteTVarC<T, fp::Unit>
    writeTVarC(const TVar<T>& tvar, const T& val)
{
    auto f = WriteTVarC<T, fp::Unit>();
    f.tvar = tvar;
    f.val  = val;
    f.next = fp::unit;
    return f;
}

// readTVar :: TVar a -> Free f a
template <typename T>
ReadTVarC<T, T>
    readTVarC(const TVar<T>& tvar)
{
    auto f = ReadTVarC<T, T>();
    f.tvar = tvar;
    f.next = fp::id;
    return f;
}

template <typename T, typename Next>
NewTVarW<Next>
    newTVarW(const T& val)
{
    NewTVarC<std::any, Next> newTVarAny;
    newTVarAny.val = std::make_any(val);

    // std::function<Next(TVar<std::any>)>
    newTVarAny.next = fp::id;

    NewTVarW<Next> f;
    f.method = newTVarAny;
    return f;
}


} // namespace stm

#endif // STM_FREE_H

