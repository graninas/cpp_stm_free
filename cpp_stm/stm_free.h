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
struct FreeF;

template <typename A>
struct PureF;

template <typename A>
STML<A> pureF(const A& x);

template <typename A>
STML<A> freeF(const STMF<STML<A>>& x);

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
    std::variant<PureF<A>, FreeF<A>> stml;
};

template <typename A>
struct FreeF
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
    freeF(const STMF<STML<A>>& x)
{
    STML<A> f;
    f.stml = FreeF<A>{x};
    return f;
}

template <typename Ret, template <typename> class Method>
STML<Ret>
    wrap(const Method<STML<Ret>>& method)
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
    n.next = [](const TVarAny& tvar){
        return pureF(tvar);
    };
    return wrap(n);
}

STML<Any>
    readTVar(const TVarAny& tvar)
{
    ReadTVarA<STML<Any>> n;
    n.tvar = tvar;
    n.next = [](const Any& any){
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



} // namespace stm

#endif // STM_STML_H

