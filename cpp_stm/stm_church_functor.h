#ifndef STM_CHURCH_FUNCTOR_H
#define STM_CHURCH_FUNCTOR_H

#include <functional>
#include <any>
#include <variant>
#include <iostream>

#include "tvar.h"
#include "stm_free_church.h"

namespace stm
{
namespace church
{
// newtype F f a = F { runF :: forall r. (a -> r) -> (f r -> r) -> r }

//instance Functor (F f) where
//    fmap f (F k) = F (\p s -> k (p . f) s)

template <typename A, typename B>
STML<B> fmap(
        const std::function<B(A)>& f,
        const STML<A>& church)
{
    STML<B> n;

    n.runF = [=](
            const std::function<free::Any(B)>& p,
            const std::function<free::Any(free::STMF<free::Any>)>& s)
    {
        auto composed = [=](const A& a)
        {
            return p(f(a));
        };

        return church.runF(composed, s);
    };

    return  n;
}

} // namespace church
} // namespace stm

#endif // STM_CHURCH_FUNCTOR_H
