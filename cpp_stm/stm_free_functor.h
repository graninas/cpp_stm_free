#ifndef STM_FREE_FUNCTOR_H
#define STM_FREE_FUNCTOR_H

#include <functional>
#include <any>
#include <variant>
#include <iostream>

#include "tvar.h"
#include "stm_free.h"

namespace stm
{
namespace free
{

template <typename A, typename B>
using MapFunc = std::function<B(A)>;

template <typename A, typename B>
struct StmfFunctorVisitor
{
    MapFunc<A,B> fTemplate;
    STMF<B> result;

    StmfFunctorVisitor(const MapFunc<A,B>& func)
        : fTemplate(func)
    {}

    void operator()(const NewTVarA<A>& fa)
    {
        MapFunc<A,B> g = fTemplate;
        NewTVarA<B> fb;
        fb.val = fa.val;
        fb.name = fa.name;
        fb.next = [=](const TVarAny& tvar)
        {
            return g(fa.next(tvar));
        };
        result.stmf = fb;
    }

    void operator()(const ReadTVarA<A>& fa)
    {
        MapFunc<A,B> g = fTemplate;
        ReadTVarA<B> fb;
        fb.tvar = fa.tvar;
        fb.next = [=](const Any& val)
        {
            return g(fa.next(val));
        };
        result.stmf = fb;
    }

    void operator()(const WriteTVarA<A>& fa)
    {
        MapFunc<A,B> g = fTemplate;
        WriteTVarA<B> fb;
        fb.tvar = fa.tvar;
        fb.val = fa.val;
        fb.next = [=](const Unit& u)
        {
            return g(fa.next(u));
        };
        result.stmf = fb;
    }

    void operator()(const RetryA<A>&)
    {
        result.stmf = RetryA<B> {};
    }
};

template <typename A, typename B>
STMF<B> fmap(const MapFunc<A, B>& f,
             const STMF<A>& method)
{
    StmfFunctorVisitor<A, B> visitor(f);
    std::visit(visitor, method.stmf);
    return visitor.result;
}

} // namespace free
} // namespace stm

#endif // STM_FREE_FUNCTOR_H
