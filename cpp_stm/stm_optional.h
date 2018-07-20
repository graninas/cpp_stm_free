#ifndef STM_OPTIONAL_H
#define STM_OPTIONAL_H

#include <tuple>
#include <optional>

#include <stm.h>

namespace stm
{

/// STML monadic interface
// Optional-aware functions

template <typename A, typename B>
STML<B> withOptional(const STML<std::optional<A>>& opt,
                     const STML<B>& onNullOpt,
                     const std::function<STML<B>(A)>& f)
{
    return stm::bind<std::optional<A>, B>(opt, [=](const std::optional<A>& optA)
    {
        return optA.has_value() ? f(optA.value()) : onNullOpt;
    });
}


template <typename A>
STML<std::optional<A>> tryModifyTVar(
        const TVar<A>& tvar,
        const std::function<std::optional<A>(A)>& f)
{
    return withTVar<A, std::optional<A>>(tvar, [=](const A& a)
    {
        std::optional<A> optNewA = f(a);
        return optNewA.has_value()
                ? sequence<Unit, std::optional<A>>(writeTVar<A>(tvar, optNewA.value()),
                                                   pure<std::optional<A>>(optNewA))
                : pure<std::optional<A>>(optNewA);
    });
}


} // namespace stm

#endif // STM_OPTIONAL_H
