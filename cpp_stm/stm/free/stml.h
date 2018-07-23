#ifndef STM_FREE_STML_H
#define STM_FREE_STML_H

#include "../tvar.h"
#include "../types.h"
#include "../stmf/stmf.h"
#include "../impl/runtime.h"

#ifdef STM_DEBUG
#include <iostream>
#endif

namespace stm
{
namespace free
{
// STM Free

// Forward declaration
template <typename A>
struct STML;

template <typename A, typename B>
using ArrowFunc = std::function<STML<B>(A)>;

// forward declaration for bind
template <typename A, typename B>
struct BindStmlVisitor;

// forward declaration for runner
template <typename Ret>
struct StmlVisitor;

template <typename Ret, template <typename> class Visitor>
RunResult<Ret> runSTML(AtomicRuntime& runtime, const STML<Ret>& stml);

// Free methods

template <typename Ret>
struct PureF
{
    Ret ret;
};

template <typename Ret>
struct FreeF
{
    stmf::STMF<STML<Ret>> stmf;
};

// Recursive Free STML algebraic data type

template <typename A>
struct STML
{
    std::variant<PureF<A>, FreeF<A>> stml;

    template <typename B>
// TODO - &
//    static STML<B> bind(const STML<A>& ma,
    static STML<B> bind(const STML<A> ma,
                        const std::function<STML<B>(A)>& f)
           {
               BindStmlVisitor<A, B> visitor(f);
               std::visit(visitor, ma.stml);
               return visitor.result;
           }

    template <template <typename, typename> class Method>
    static STML<A> wrap(const Method<Any, STML<A>>& method)
    {
        return { FreeF<A> { stmf::STMF<STML<A>> { method } } };
    }

    static STML<A> pure(const A& a)
    {
        return { PureF<A>{ a } };
    }

    static STML<TVar<A>> newTVar(const A& val,
                                 const std::string& name = "")
    {
        auto r = stmf::NewTVar<A, STML<TVar<A>>>::toAny(
                    val,
                    name,
                    [](const TVar<A>& tvar)
                        { return STML<TVar<A>>::pure(tvar); }
                    );
        return STML<TVar<A>>::wrap(r);
    }

    static STML<A> readTVar(const TVar<A>& tvar)
    {
        auto r = stmf::ReadTVar<A, STML<A>>::toAny(
                    tvar,
                    [](const A& a)
                        { return STML<A>::pure(a); }
                    );
        return STML<A>::wrap(r);
    }

    static STML<Unit> writeTVar(const TVar<A>& tvar,
                                const A& val)
    {
        auto r = stmf::WriteTVar<A, STML<Unit>>::toAny(
                    tvar,
                    val,
                    [](const Unit&)
                        { return STML<Unit>::pure(unit); }
                    );
        return STML<Unit>::wrap(r);
    }

    static STML<A> retry()
    {
        return STML<A>::wrap(stmf::RetryA<STML<A>> {});
    }

    static A atomically(
            Context& context,
            const STML<A>& stml)
    {
        RunnerFunc<A> runner = [&](AtomicRuntime& runtime)
        {
            return runSTML<A, StmlVisitor>(runtime, stml);
        };

        return runSTM<A>(context, runner);
    }
};

} // namespace free
} // namespace stm

#endif // STM_FREE_STML_H

