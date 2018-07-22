#ifndef STM_CHURCH_H
#define STM_CHURCH_H

#include <tuple>

#include "context.h"
#include "stm_free_church.h"
#include "stm_free_functor.h"

namespace stm
{
namespace church
{

//-- Church encoding of Free monad
//newtype F f a = F { runF :: forall r. (a -> r) -> (f r -> r) -> r }


//lift :: Functor f => f a -> F f a
//lift f = F (\p r -> r (fmap p f))


template <typename A, template <typename, typename> class Method>
STML<A> wrapChurch(const Method<free::Any, A>& method)
{
    STML<A> n;

    n.runF = [=](const std::function<free::Any(A)>& p,
                 const std::function<free::Any(free::STMF<free::Any>)>& r)
    {
        free::STMF<A> f { method };
        free::STMF<free::Any> mapped = free::fmap(p, f);
        return r(mapped);
    };

    return n;
}

template <typename A>
STML<TVar<A>> newTVar(
        const A& val,
        const std::string& name = "")
{
    free::NewTVar<free::Any, TVar<A>> r =
            free::NewTVar<A, TVar<A>>::toAny(
                val,
                name,
                [](const TVar<A>& tvar) { return tvar; }
                );
    return wrapChurch(r);
}

/// STML monadic interface

//instance Functor (F f) where
//    fmap f (F k) = F (\p -> k (p . f))

//instance Monad (F f) where
//    return a = F (\p _ -> p a)
//    (F k) >>= f = F (\p r -> k (\a -> runF (f a) p r) r)

//std::function<
//    Any(
//        std::function<Any(A)>,
//        std::function<Any(STMF<Any>)>
//        > runF;

template <typename A, typename B>
STML<B> bind(
        const STML<A> ma,
        const std::function<STML<B>(A)>& f)
{
    STML<B> n;
    n.runF = [=](const std::function<free::Any(B)>& p,
                 const std::function<free::Any(free::STMF<free::Any>)>& r)
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

template <typename A>
STML<A> pure(const A& a)
{
    STML<A> n;
    n.runF = [=](const std::function<free::Any(A)>& p,
                 const std::function<free::Any(free::STMF<free::Any>)>&)
    {
        return p(a);
    };
    return n;
}

} // namespace church
} // namespace stm

#endif // STM_CHURCH_H
