#ifndef STM_CHURCH_STML_H
#define STM_CHURCH_STML_H

#include "../tvar.h"
#include "../types.h"
#include "../stmf/stmf.h"
#include "../stmf/functor.h"
//#include "bind.h"
#include "../impl/runtime.h"

#ifdef STM_DEBUG
#include <iostream>
#endif

namespace stm
{
namespace church
{

// STM Free Church-encoded

// Church STML algebraic data type

// forward declaration
template <typename A>
struct STML;

//// forward declaration for runner
template <typename A>
struct StmfVisitor;

template <typename A>
RunResult<A> runSTML(AtomicRuntime& runtime,
                     const STML<A>& stml);

// newtype F f a = F { runF :: forall r. (a -> r) -> (f r -> r) -> r }

template <typename A>
struct STML
{
    std::function<
        Any(
            std::function<Any(A)>,
            std::function<Any(stmf::STMF<Any>)>
        )> runF;


    template <template <typename, typename> class Method>
    static STML<A> wrap(const Method<Any, A>& method)
    {
        STML<A> n;

        n.runF = [=](const std::function<Any(A)>& p,
                     const std::function<Any(stmf::STMF<Any>)>& r)
        {
            stmf::STMF<A> f { method };
            stmf::STMF<Any> mapped = stmf::fmap<A, Any>(p, f);
            return r(mapped);
        };

        return n;
    }

    static STML<TVar<A>> newTVar(
            const A& val,
            const std::string& name = "")
    {
        auto r = stmf::NewTVar<A, TVar<A>>::toAny(
                    val,
                    name,
                    [](const TVar<A>& tvar) { return tvar; }
                    );

        return wrap(r);
    }

    static STML<A> readTVar(const TVar<A>& tvar)
    {
        auto r = stmf::ReadTVar<A, A>::toAny(
                    tvar,
                    [](const A& val) { return val;  });

        return wrap(r);
    }

    static STML<Unit> writeTVar(const TVar<A>& tvar,
                         const A& val)
    {
        auto r = stmf::WriteTVar<A, Unit>::toAny(
                    tvar,
                    val,
                    [](const Unit& unit) { return unit; }
                    );
        return wrap(r);
    }

    static STML<A> retry()
    {
        return STML<A>::wrap(stmf::RetryA<A> {});
    }

    template <typename B>
    static STML<B> bind(
// TODO: &
//            const STML<A>& ma,
            const STML<A> ma,
            const std::function<STML<B>(A)>& f)
    {
        STML<B> n;
        n.runF = [=](const std::function<Any(B)>& p,
                     const std::function<Any(stmf::STMF<Any>)>& r)
        {
            auto fst = [=](const A& a)
            {
                STML<B> internal = f(a);
                return internal.runF(p, r);
            };

            return ma.runF(fst, r);
        };
        return n;
    }

    static STML<A> pure(const A& a)
    {
        STML<A> n;
        n.runF = [=](const std::function<Any(A)>& p,
                     const std::function<Any(stmf::STMF<Any>)>&)
        {
            return p(a);
        };
        return n;
    }

    static A atomically(
            Context& context,
            const STML<A>& stml)
    {
        RunnerFunc<A> runner = [&](AtomicRuntime& runtime)
        {
            return runSTML<A>(runtime, stml);
        };

        return runSTM<A>(context, runner);
    }
};

// Wrappers

//template <typename Ret, template <typename, typename> class Method>
//STML<Ret> wrapA(const Method<Any, STML<Ret>>& method)
//{
//    return { FreeF<Ret> { STMF<STML<Ret>> { method } } };
//}

} // namespace church
} // namespace stm

#endif // STM_CHURCH_STML_H

