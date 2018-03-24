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

template <typename A, typename B>
using ArrowFunc = std::function<STML<B>(A)>;

template <typename A, typename B, template <typename, typename> class Visitor>
STML<B> runBind(const STML<A>& stml, const ArrowFunc<A, B>& f)
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
    ArrowFunc<A,B> fTemplate;
    STMF<STML<B>> result;

    BindStmfVisitor(const ArrowFunc<A,B>& func)
        : fTemplate(func)
    {}

    void operator()(const NewTVarA<STML<A>>& fa)
    {
        std::cout << "\nBind: NewTVarA";

        ArrowFunc<A,B> f = fTemplate;
        NewTVarA<STML<B>> fb;
        fb.val = fa.val;
        fb.next = [=](const TVarAny& tvar)
        {
            STML<A> nextA = fa.next(tvar);
            return runBind<A, B, BindStmlVisitor>(nextA, f);
        };
        result.stmf = fb;
    }

    void operator()(const ReadTVarA<STML<A>>& fa)
    {
        std::cout << "\nBind: ReadTVarA.";

        ArrowFunc<A,B> f = fTemplate;
        ReadTVarA<STML<B>> fb;
        fb.tvar = fa.tvar;
        fb.next = [=](const Any& val)
        {
            STML<A> nextA = fa.next(val);
            return runBind<A, B, BindStmlVisitor>(nextA, f);
        };
        result.stmf = fb;
    }

    void operator()(const WriteTVarA<STML<A>>& fa)
    {
        std::cout << "\nBind: WriteTVarA.";

        ArrowFunc<A,B> f = fTemplate;
        WriteTVarA<STML<B>> fb;
        fb.tvar = fa.tvar;
        fb.val = fa.val;
        fb.next = [=](const fp::Unit&)
        {
            STML<A> nextA = fa.next(fp::unit);
            return runBind<A, B, BindStmlVisitor>(nextA, f);
        };
        result.stmf = fb;
    }

    void operator()(const RetryA<STML<A>>&)
    {
        std::cout << "\nBind: RetryA.";

        RetryA<STML<B>> fb;
        result.stmf = fb;
    }
};

template <typename A, typename B>
struct BindStmlVisitor
{
    ArrowFunc<A,B> fTemplate;
    STML<B> result;

    BindStmlVisitor(const ArrowFunc<A,B>& func)
        : fTemplate(func)
    {}

    void operator()(const PureF<A>& fa)
    {
        std::cout << "\nBind: PureF";

        ArrowFunc<A,B> f = fTemplate;
        result = f(fa.ret);
    }

    void operator()(const FreeF<A>& fa)
    {
        std::cout << "\nBind: FreeF";

        ArrowFunc<A,B> f = fTemplate;
        BindStmfVisitor<A, B> visitor(f);
        std::visit(visitor, fa.stmf.stmf);
        STMF<STML<B>> visited = visitor.result;
        result.stml = FreeF<B> { visited };
    }
};

template <typename A, typename B>
STML<B> bind(const STML<A> ma, const ArrowFunc<A, B>& f)
{
    BindStmlVisitor<A, B> visitor(f);
    std::visit(visitor, ma.stml);
    return visitor.result;
}

}

#endif // STM_BIND_H
