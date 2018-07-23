#ifndef STM_FREE_BIND_H
#define STM_FREE_BIND_H

#include "stml.h"

namespace stm
{
namespace free
{

// forward declaration for bind
template <typename A, typename B>
struct BindStmlVisitor;

template <typename A, typename B>
using ArrowFunc = std::function<STML<B>(A)>;

template <typename A, typename B,
          template <typename, typename> class Visitor>
STML<B> runBind(const STML<A>& stml, const ArrowFunc<A, B>& f)
{
    Visitor<A, B> visitor(f);
    std::visit(visitor, stml.stml);
    return visitor.result;
}


template <typename A, typename B>
struct BindStmfVisitor
{
    ArrowFunc<A,B> fTemplate;
    stmf::STMF<STML<B>> result;

    BindStmfVisitor(const ArrowFunc<A,B>& func)
        : fTemplate(func)
    {}

    void operator()(const stmf::NewTVarA<STML<A>>& fa)
    {
//        std::cout << "\nBind: NewTVarA";

        ArrowFunc<A,B> f = fTemplate;
        stmf::NewTVarA<STML<B>> fb;
        fb.val = fa.val;
        fb.name = fa.name;
        fb.next = [=](const TVarAny& tvar)
        {
            STML<A> nextA = fa.next(tvar);
            return runBind<A, B, BindStmlVisitor>(nextA, f);
        };
        result.stmf = fb;
    }

    void operator()(const stmf::ReadTVarA<STML<A>>& fa)
    {
//        std::cout << "\nBind: ReadTVarA.";

        ArrowFunc<A,B> f = fTemplate;
        stmf::ReadTVarA<STML<B>> fb;
        fb.tvar = fa.tvar;
        fb.next = [=](const Any& val)
        {
            STML<A> nextA = fa.next(val);
            return runBind<A, B, BindStmlVisitor>(nextA, f);
        };
        result.stmf = fb;
    }

    void operator()(const stmf::WriteTVarA<STML<A>>& fa)
    {
//        std::cout << "\nBind: WriteTVarA.";

        ArrowFunc<A,B> f = fTemplate;
        stmf::WriteTVarA<STML<B>> fb;
        fb.tvar = fa.tvar;
        fb.val = fa.val;
        fb.next = [=](const Unit&)
        {
            STML<A> nextA = fa.next(unit);
            return runBind<A, B, BindStmlVisitor>(nextA, f);
        };
        result.stmf = fb;
    }

    void operator()(const stmf::RetryA<STML<A>>&)
    {
//        std::cout << "\nBind: RetryA.";
        result.stmf = stmf::RetryA<STML<B>> {};
    }
};

template <typename A, typename B>
struct BindStmlVisitor
{
    ArrowFunc<A, B> fTemplate;
    STML<B> result;

    BindStmlVisitor(const ArrowFunc<A,B>& func)
        : fTemplate(func)
    {}

    void operator()(const PureF<A>& fa)
    {
//        std::cout << "\nBind: PureF";

        ArrowFunc<A,B> f = fTemplate;
        result = f(fa.ret);
    }

    void operator()(const FreeF<A>& fa)
    {
//        std::cout << "\nBind: FreeF";

        ArrowFunc<A,B> f = fTemplate;
        BindStmfVisitor<A, B> visitor(f);
        std::visit(visitor, fa.stmf.stmf);
        stmf::STMF<STML<B>> visited = visitor.result;
        result.stml = FreeF<B> { visited };
    }
};

} // namespace free
} // namespace stm

#endif // STM_FREE_BIND_H
