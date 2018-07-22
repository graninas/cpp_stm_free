#ifndef STM_FREE_CHURCH_H
#define STM_FREE_CHURCH_H

#include <functional>
#include <any>
#include <variant>

#include "stm_free.h"

#ifdef STM_DEBUG
#include <iostream>
#endif

namespace stm
{
namespace church
{

// STM Free Church-encoded

// Church STML algebraic data type

// newtype F f a = F { runF :: forall r. (a -> r) -> (f r -> r) -> r }

template <typename A>
struct STML
{
    std::function<
        free::Any(
            std::function<free::Any(A)>,
            std::function<free::Any(free::STMF<free::Any>)>
        )> runF;
};

// Wrappers

//template <typename Ret, template <typename, typename> class Method>
//STML<Ret> wrapA(const Method<Any, STML<Ret>>& method)
//{
//    return { FreeF<Ret> { STMF<STML<Ret>> { method } } };
//}

} // namespace church
} // namespace stm

#endif // STM_FREE_CHURCH_H

