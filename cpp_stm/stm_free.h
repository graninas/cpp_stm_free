#ifndef STM_STML_H
#define STM_STML_H

#include <functional>
#include <any>
#include <variant>
#include <iostream>

#include <unit.h>
#include <identity.h>

#include "tvar.h"

namespace stm
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
    std::function<Next(TVar<A>)> next;
};

template <typename Next>
using NewTVarA = NewTVar<Any, Next>;

template <typename A, typename Next>
struct ReadTVar
{
    TVar<A> tvar;
    std::function<Next(A)> next;
};

template <typename Next>
using ReadTVarA = ReadTVar<Any, Next>;

template <typename A, typename Next>
struct WriteTVar
{
    TVar<A> tvar;
    A val;
    std::function<Next(fp::Unit)> next;
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
    wrap(const Method<Any, STML<Ret>>& method)
{
    STMF<STML<Ret>> f {method};
    FreeF<Ret> b {f};
    return {b};
}

STML<TVarAny>
    newTVar(const Any& val)
{
    NewTVarA<STML<TVarAny>> n;
    n.val = val;
    n.next = [](const TVarAny& tvar) {
        return pureF(tvar);
    };
    return wrap(n);
}

// Experiments
//template <typename A>
//STML<TVar<A>>
//    newTVar(const A& val)
//{
//    NewTVarA<STML<TVarAny>> n;
//    n.val = val;
//    n.next = [=](const TVarAny& tvar){
//        TVar<A> tvar2;
//        tvar2.id = tvar;
//        return pureF(tvar2);
//    };
//    return wrap(n);
//}

STML<Any>
    readTVar(const TVarAny& tvar)
{
    ReadTVarA<STML<Any>> n;
    n.tvar = tvar;
    n.next = [](const Any any){
        return pureF(any);
    };
    return wrap(n);
}

STML<fp::Unit>
    writeTVar(const TVarAny& tvar, const Any& val)
{
    WriteTVarA<STML<fp::Unit>> n;
    n.tvar = tvar;
    n.val = val;
    n.next = [](const fp::Unit& unit){
        return pureF(unit);
    };
    return wrap(n);
}

template <typename AnyRet>
STML<AnyRet>
    retry()
{
    RetryA<STML<AnyRet>> n;
    return wrap(n);
}

} // namespace stm

#endif // STM_STML_H

