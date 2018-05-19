#ifndef STM_FREE_H
#define STM_FREE_H

#include <functional>
#include <any>
#include <variant>
#include <iostream>

#include <unit.h>
#include <identity.h>

#include "tvar.h"

namespace stm
{

namespace free
{
// STM Free

// Short definitions
using Any = std::any;
using TVarAny = TVar<std::any>;

// Forward declaration, in stm namespace

template <typename A>
struct STML;

// STM methods

template <typename A, typename Next>
struct NewTVar
{
    A val;
    std::string name;
    std::function<Next(TVar<A>)> next;

    NewTVar<Any, Next> toAny() const
    {
        std::function<Next(TVar<A>)> nextCopy = next;

        NewTVar<Any, Next> m;
        m.val = val;
        m.name = name;
        m.next = [=](const TVarAny& tvarAny)
        {
            TVar<A> tvar;
            tvar.id = tvarAny.id;
            tvar.name = tvarAny.name;
            return nextCopy(tvar);
        };
        return m;
    }
};

template <typename A, typename Next>
struct ReadTVar
{
    TVar<A> tvar;
    std::function<Next(A)> next;

    ReadTVar<Any, Next> toAny() const
    {
        std::function<Next(A)> nextCopy = next;
        TVar<Any> tvar2;
        tvar2.id = tvar.id;
        tvar2.name = tvar.name;

        ReadTVar<Any, Next> m;
        m.tvar = tvar2;
        m.next = [=](const Any& val)
        {
            A val2 = std::any_cast<A>(val);
            return nextCopy(val2);
        };
        return m;
    }
};

template <typename A, typename Next>
struct WriteTVar
{
    TVar<A> tvar;
    A val;
    std::function<Next(fp::Unit)> next;

    WriteTVar<Any, Next> toAny() const
    {
        std::function<Next(fp::Unit)> nextCopy = next;
        TVar<Any> tvar2;
        tvar2.id = tvar.id;
        tvar2.name = tvar.name;

        WriteTVar<Any, Next> m;
        m.tvar = tvar2;
        m.val  = val;
        m.next = [=](const fp::Unit& unit)
        {
            return nextCopy(unit);
        };
        return m;
    }
};

template <typename A, typename Next>
struct Retry
{
};

// Any-specified STM methods

template <typename Next>
using NewTVarA = NewTVar<Any, Next>;

template <typename Next>
using ReadTVarA = ReadTVar<Any, Next>;

template <typename Next>
using WriteTVarA = WriteTVar<Any, Next>;

template <typename Next>
using RetryA = Retry<Any, Next>;

// STMF algebraic data type

template <class Ret>
struct STMF
{
    std::variant<NewTVarA<Ret>, ReadTVarA<Ret>, WriteTVarA<Ret>, RetryA<Ret>> stmf;
};

// Free methods

template <typename Ret>
struct PureF
{
    Ret ret;
};

template <typename Ret>
struct FreeF
{
    STMF<STML<Ret>> stmf;
};

// Recursive Free STML algebraic data type

template <typename Ret>
struct STML
{
    std::variant<PureF<Ret>, FreeF<Ret>> stml;
};

// Wrappers

template <typename Ret>
STML<Ret> pureF(const Ret& a)
{
    return { PureF<Ret> { a } };
}

template <typename A, typename Ret, template <typename, typename> class Method>
STML<Ret> wrapT(const Method<A, STML<Ret>>& method)
{
//    Method<Any, STML<Ret>> method2;
//    method2 = method.toAny();

//    STMF<STML<Ret>> f {method2};
//    FreeF<Ret> b {f};
//    return {b};
    return { FreeF<Ret> { STMF<STML<Ret>> { method.toAny() } } };
}

template <typename Ret, template <typename, typename> class Method>
STML<Ret> wrapA(const Method<Any, STML<Ret>>& method)
{
//    STMF<STML<Ret>> f {method};
//    FreeF<Ret> b {f};
//    return {b};
    return { FreeF<Ret> { STMF<STML<Ret>> { method } } };
}

} // namespace free
} // namespace stm

#endif // STM_FREE_H

