#ifndef STM_H
#define STM_H

#include "context.h"
#include "stm_runtime.h"
#include "stm_free.h"
#include "stm_bind.h"
#include "tvar.h"

namespace stm
{

template <typename A>
using STML = free::STML<A>;

template <typename A>
A atomically(Context& context, const STML<A>& stml)
{
    return runSTM(context, stml);
}

const auto newTVar = [](const auto& val)
{
    return free::newTVar(val);
};

const auto readTVar = [](const auto& tvar)
{
    return free::readTVar(tvar);
};

const auto writeTVar = [](const auto& tvar)
{
    return [&](const auto& val)
    {
        return free::writeTVar(tvar, val);
    };
};

const auto writeTVarV = [](const auto& val)
{
    return [&](const auto& tvar)
    {
        return free::writeTVar(tvar, val);
    };
};

const auto retry = [](const auto&)
{
    return free::retry<fp::Unit>();
};

const auto pure = [](const auto& a)
{
    return free::pureF(a);
};

template <typename A, typename B>
STML<B> bind(const STML<A> ma, const free::ArrowFunc<A, B>& f)
{
    return free::bind(ma, f);
}

} // namespace stm

#endif // STM_H
