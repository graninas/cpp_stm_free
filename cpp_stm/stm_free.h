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

using TVarAny = TVar<std::any>;
using Any = std::any;

// STM Free

// Forward declaration
template <typename A>
struct STML;

template <class Next>
struct STMF;

template <typename Next>
struct FreeF;

template <typename A>
struct PureF;

template <typename A>
STML<A> pureF(const A& x);

template <typename A>
STML<A> freeF(const STMF<STML<A>>& x);

// STML

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

template <typename Next>
using NewTVarA = NewTVar<Any, Next>;

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

template <typename Next>
using ReadTVarA = ReadTVar<Any, Next>;

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

template <typename Next>
using WriteTVarA = WriteTVar<Any, Next>;

template <typename A, typename Next>
struct Retry
{
};

template <typename Next>
using RetryA = Retry<Any, Next>;

template <class Ret>
struct STMF
{
    std::variant<NewTVarA<Ret>, ReadTVarA<Ret>, WriteTVarA<Ret>, RetryA<Ret>> stmf;
};

// Ad-hoc STML

template <typename Ret>
struct PureF
{
    Ret ret;
};

template <typename Ret>
STML<Ret>
    pureF(const Ret& a)
{
    STML<Ret> f;
    f.stml = PureF<Ret>{a};
    return f;
}

template <typename Ret>
Ret unPureF(const PureF<Ret>& p)
{
    return p.ret;
}

template <typename Ret>
struct STML
{
    using RetT = Ret;
    std::variant<PureF<Ret>, FreeF<Ret>> stml;
};

template <typename Ret>
struct FreeF
{
    STMF<STML<Ret>> stmf;
};

template <typename Ret>
STML<Ret>
    freeF(const STMF<STML<Ret>>& stmf)
{
    STML<Ret> f;
    f.stml = FreeF<Ret>{stmf};
    return f;
}

template <typename Ret, template <typename, typename> class Method>
STML<Ret>
    wrapA(const Method<Any, STML<Ret>>& method)
{
    STMF<STML<Ret>> f {method};
    FreeF<Ret> b {f};
    return {b};
}

STML<TVarAny>
    newTVarA(const Any& val, const std::string& name = "")
{
    NewTVarA<STML<TVarAny>> n;
    n.val = val;
    n.name = name;
    n.next = [](const TVarAny& tvar) {
        return pureF(tvar);
    };
    return wrapA(n);
}

STML<Any>
    readTVarA(const TVarAny& tvar)
{
    ReadTVarA<STML<Any>> n;
    n.tvar = tvar;
    n.next = [](const Any any){
        return pureF(any);
    };
    return wrapA(n);
}

STML<fp::Unit>
    writeTVarA(const TVarAny& tvar, const Any& val)
{
    WriteTVarA<STML<fp::Unit>> n;
    n.tvar = tvar;
    n.val = val;
    n.next = [](const fp::Unit& unit){
        return pureF(unit);
    };
    return wrapA(n);
}

template <typename AnyRet>
STML<AnyRet>
    retry()
{
    RetryA<STML<AnyRet>> n;
    return wrapA(n);
}

// Experiments
template <typename A, typename Ret, template <typename, typename> class Method>
STML<Ret>
    wrapT(const Method<A, STML<Ret>>& method)
{
    Method<Any, STML<Ret>> method2;
    method2 = method.toAny();

    STMF<STML<Ret>> f {method2};
    FreeF<Ret> b {f};
    return {b};
}

template <typename A>
STML<TVar<A>>
    newTVar(const A& val, const std::string& name = "")
{
    NewTVar<A, STML<TVar<A>>> n;
    n.val = val;
    n.name = name;
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
    free::WriteTVar<A, STML<fp::Unit>> n;
    n.tvar = tvar;
    n.val  = val;
    n.next = [](const fp::Unit& unit) {
        return pureF(unit);
    };
    return wrapT(n);
}

} // namespace free
} // namespace stm

#endif // STM_FREE_H

