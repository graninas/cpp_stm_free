#ifndef STM_BIND_H
#define STM_BIND_H

#include <functional>
#include <any>
#include <variant>
#include <iostream>

#include <unit.h>
#include <identity.h>

#include "tvar.h"
#include "stm_free.h"

namespace stm
{


template <typename A, typename B, template <typename, typename> class Visitor>
STML<B> runBind(const STML<A>& stml, const std::function<STML<B>(A)>& f)
{
    Visitor<A, B> visitor(f);
    std::visit(visitor, stml.stml);
    return visitor.result;
}

// forward declaration;
template <typename A, typename B>
struct BindStmlVisitor;

template <typename A, typename B>
struct BindStmfVisitor
{
    std::function<STML<B>(A)> f;
    STMF<STML<B>> result;

    BindStmfVisitor(const std::function<STML<B>(A)>& func)
        : f(func)
    {}

    void operator()(const NewTVarA<STML<A>>& fa)
    {
        std::cout << "\nBind: NewTVarA";

        NewTVarA<STML<B>> fb;
        fb.val = fa.val;
        fb.next = [&](const TVarAny& tvar)
        {
            STML<A> nextA = fa.next(tvar);
            return runBind<A, B, BindStmlVisitor>(nextA, f);
        };
        result.stmf = fb;
    }

    void operator()(const ReadTVarA<STML<A>>& fa)
    {
        std::cout << "\nBind: ReadTVarA.";

        ReadTVarA<STML<B>> fb;
        fb.tvar = fa.tvar;
        fb.next = [&](const Any& val)
        {
            STML<A> nextA = fa.next(val);
            return runBind<A, B, BindStmlVisitor>(nextA, f);
        };
        result.stmf = fb;
    }

    void operator()(const WriteTVarA<STML<A>>& fa)
    {
        std::cout << "\nBind: WriteTVarA.";

        WriteTVarA<STML<B>> fb;
        fb.tvar = fa.tvar;
        fb.val = fa.val;
        fb.next = [&](const fp::Unit&)
        {
            STML<A> nextA = fa.next(fp::unit);
            return runBind<A, B, BindStmlVisitor>(nextA, f);
        };
        result.stmf = fb;
    }
};

template <typename A, typename B>
struct BindStmlVisitor
{
    std::function<STML<B>(A)> f;
    STML<B> result;

    BindStmlVisitor(const std::function<STML<B>(A)>& func)
        : f(func)
    {}

    void operator()(const PureF<A>& fa)
    {
        std::cout << "\nBind: PureF";

        result = f(fa.ret);
    }

    void operator()(const FreeF<A>& fa)
    {
        std::cout << "\nBind: FreeF";

        BindStmfVisitor<A, B> visitor(f);
        std::visit(visitor, fa.stmf.stmf);
        STMF<STML<B>> visited = visitor.result;
        result.stml = FreeF<B> { visited };
    }
};

/*
\val -> NewTVar val
                \tvar -> ReadTVar tvar
                                  \val2 -> pure val2

\val -> NewTVarA val
                 \tvar -> ReadTVarA tvar
                                    \val2 -> PureF val2

newTVar val :: STML TVarAny
    >>= \tvar -> readTVar tvar :: STML Any
                      >>= \val2 -> pure val2 :: STML Any

newTVar val :: STML<TVarAny>
    >>= \tvar -> readTVar tvar :: STML<Any>
                      >>= \val2 -> pure val2 :: STML<Any>


>>= :: STML<a> -> (a -> STML<b>) -> STML<b>

*/



template <typename A, typename B>
STML<B> bind(const STML<A>& ma, const std::function<STML<B>(A)>& f)
{
    /*

    newTVar :: STML<TVar>
    next :: TVar -> STML<TVar>

    NewTVarA<STML<B>> method;
    method.val = val;
    method.next = [&](TVar tvar)
    {
        return f(tvar);
    };

    NewTVarA 10 >>= \tvar -> ReadTVar

      */

    BindStmlVisitor<A, B> visitor(f);
    std::visit(visitor, ma.stml);
    return visitor.result;
}

}

#endif // STM_BIND_H
