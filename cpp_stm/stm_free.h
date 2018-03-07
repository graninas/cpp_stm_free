#ifndef STM_STML_H
#define STM_STML_H

#include <functional>
#include <any>
#include <variant>

#include <unit.h>
#include <identity.h>

#include "tvar.h"

namespace stm
{

#define TVarAny TVar<std::any>
#define Any std::any

// STM Free

// Forward declaration
template <typename A>
struct STML;

template <class Next>
struct STMF;

template <typename Next>
struct BindF;

template <typename A>
struct PureF;

template <typename A>
STML<A> pureF(const A& x);

template <typename A>
STML<A> bindF(const STMF<STML<A>>& x);

// STML

template <typename Next>
struct NewTVarA
{
    std::any val;
    std::function<Next(TVarAny)> next;
};

NewTVarA<TVarAny>
    newTVarA(const std::any& val)
{
    NewTVarA<TVarAny> f;
    f.val = val;
    f.next = fp::id;
    return f;
}

template <typename Next>
struct ReadTVarA
{
    TVarAny tvar;
    std::function<Next(std::any)> next;
};

ReadTVarA<std::any>
    readTVarA(const TVarAny& tvar)
{
    ReadTVarA<std::any> f;
    f.tvar = tvar;
    f.next = fp::id;
    return f;
}

template <typename Next>
struct WriteTVarA
{
    TVarAny tvar;
    std::any val;
    std::function<Next(fp::Unit)> next;
};

WriteTVarA<fp::Unit>
    writeTVarA(const TVarAny& tvar, const std::any& val)
{
    WriteTVarA<fp::Unit> f;
    f.tvar = tvar;
    f.val  = val;
    f.next = fp::id;
    return f;
}

template <class Next>
struct STMF
{
    std::variant<NewTVarA<Next>, ReadTVarA<Next>, WriteTVarA<Next>> stmf;
};

// Ad-hoc STML

template <typename A>
struct PureF
{
    A ret;
};

template <typename A>
struct STML
{
    std::variant<PureF<A>, BindF<A>> stml;
};

template <typename A>
struct BindF
{
    STMF<STML<A>> x;
};

template <typename A>
STML<A>
    pureF(const A& x)
{
    STML<A> f;
    f.stml = PureF<A>{x};
    return f;
}

template <typename A>
STML<A>
    bindF(const STMF<STML<A>>& x)
{
    STML<A> f;
    f.stml = BindF<A>{x};
    return f;
}

STML<TVarAny>
    newTVar(const std::any& val)
{
    NewTVarA<STML<TVarAny>> n;
    n.val = val;
    n.next = [](const TVarAny& tvar){
        return pureF(tvar);
    };

    STMF<STML<TVarAny>> f;
    f.stmf = n;

    BindF<TVarAny> b;
    b.x = f;

    STML<TVarAny> f2;
    f2.stml = b;

    return f2;
}

STML<Any>
    readTVar(const TVarAny& tvar)
{
    ReadTVarA<STML<Any>> n;
    n.tvar = tvar;
    n.next = [](const Any& any){
        return pureF(any);
    };

    STMF<STML<Any>> f;
    f.stmf = n;

    BindF<Any> b;
    b.x = f;

    STML<Any> f2;
    f2.stml = b;

    return f2;
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

    STMF<STML<fp::Unit>> f;
    f.stmf = n;

    BindF<fp::Unit> b;
    b.x = f;

    STML<fp::Unit> f2;
    f2.stml = b;

    return f2;
}



} // namespace stm

#endif // STM_STML_H

