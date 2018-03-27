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

const auto writeTVar = [](const auto& tvar, const auto& val)
{
    return free::writeTVar(tvar, val);
};

const auto writeTVarT = [](const auto& tvar)
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

const auto modifyTVar = [](const auto& tvar, const auto& f)
{
    auto m = free::readTVar(tvar);
    return bind(m, writeTVarT(tvar));
};

const auto modifyTVarT = [](const auto& tvar)
{
    return [&](const auto& f)
    {
        auto m = free::readTVar(tvar);
        return free::bind(m, [=](const auto& val)
        {
            return free::writeTVar(tvar, f(val));
        } );
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

template <typename A, typename B, typename Ret>
STML<Ret> both(const STML<A>& ma,
               const STML<B>& mb,
               const std::function<Ret(A, B)>& f)
{
    return free::bind<A, Ret>(ma, [=](const A& a){
        return free::bind<B, Ret>(mb, [=](const B& b){
            return pure(f(a, b));
        });
    });
}

template <typename A, typename B>
STML<fp::Unit>
bothVoided(const STML<A>& ma,
           const STML<B>& mb)
{
    return both<A, B, fp::Unit>(ma, mb, [](const A&, const B&)
    {
        return fp::unit;
    });
}

// TODO: replace by var args.
template <typename A, typename B, typename Ret>
STML<Ret> bothTVars(
        const STML<TVar<A>>& ma,
        const STML<TVar<B>>& mb,
        const std::function<Ret(A, B)>& f)
{
    return both<A, B>(free::bind<TVar<A>, A>(ma, readTVar),
                      free::bind<TVar<B>, B>(mb, readTVar),
                      f);
}

template <typename A, typename B>
STML<B> voided(const STML<A> ma, const STML<B>& mb)
{
    return free::bind<A, B>(ma, [=](const A&) {
                return mb;
            });
}

const auto when = [](const STML<bool>& ma, const auto& mb)
{
    return bind(ma, [=](bool cond) {
        return cond ? mb : pure(fp::unit);
    });
};

const auto unless = [](const STML<bool>& ma, const auto& mb)
{
    return bind(ma, [=](bool cond) {
        return cond ? pure(fp::unit) : mb;
    });
};

} // namespace stm

#endif // STM_H
