#ifndef STM_CHURCH_FUNCTOR_H
#define STM_CHURCH_FUNCTOR_H

#include "stml.h"

namespace stm
{
namespace church
{

template <typename A, typename B>
STML<B> fmap(
        const std::function<B(A)>& f,
        const STML<A>& church)
{
    STML<B> n;

    n.runF = [=](
            const std::function<Any(B)>& p,
            const std::function<Any(stmf::STMF<Any>)>& s)
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
