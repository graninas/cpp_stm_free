#ifndef STM_FMAP_H
#define STM_FMAP_H

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
STML<B> runMap(const STML<A>& stml, const std::function<B(A)>& f)
{
    Visitor<A, B> visitor(f);
    std::visit(visitor, stml.stml);
    return visitor.result;
}

// forward declaration;
template <typename A, typename B>
struct FmapStmlVisitor;

template <typename A, typename B>
struct FmapStmfVisitor
{
    std::function<B(A)> f;
    STMF<STML<B>> result;

    FmapStmfVisitor(const std::function<B(A)>& func)
        : f(func)
    {}

    void operator()(const NewTVarA<STML<A>>& fa)
    {
        std::cout << "\nFmap: NewTVarA";

        NewTVarA<STML<B>> fb;
        fb.val = fa.val;
        fb.next = [&](const TVarAny& tvar)
        {
            STML<A> nextA = fa.next(tvar);
            return runMap<A, B, FmapStmlVisitor>(nextA, f);
        };
        result = fb;
    }

    void operator()(const ReadTVarA<STML<A>>& fa)
    {
        std::cout << "\nFmap: ReadTVarA.";

        ReadTVarA<STML<B>> fb;
        fb.tvar = fa.tvar;
        fb.next = [&](const Any& val)
        {
            STML<A> nextA = fa.next(val);
            return runMap<A, B, FmapStmlVisitor>(nextA, f);
        };
        result = fb;
    }

    void operator()(const WriteTVarA<STML<A>>& fa)
    {
        std::cout << "\nFmap: WriteTVarA.";

        WriteTVarA<STML<B>> fb;
        fb.tvar = fa.tvar;
        fb.val = fa.val;
        fb.next = [&](const fp::Unit&)
        {
            STML<A> nextA = fa.next(fp::unit);
            return runMap<A, B, FmapStmlVisitor>(nextA, f);
        };
        result = fb;
    }
};

template <typename A, typename B>
struct FmapStmlVisitor
{
    std::function<B(A)> f;
    STML<B> result;

    FmapStmlVisitor(const std::function<B(A)>& func)
        : f(func)
    {}

    void operator()(const PureF<A>& fa)
    {
        std::cout << "\nFmap: PureF";

        result = PureF<B> { f(fa.ret) };
    }

    void operator()(const FreeF<A>& fa)
    {
        std::cout << "\nFmap: FreeF";

        FmapStmfVisitor<A, B> visitor(f);
        std::visit(visitor, f.stmf.stmf);
        STMF<STML<B>> visited = visitor.result;
        result = FreeF<B> { visited };
    }
};


}

#endif // STM_FMAP_H
