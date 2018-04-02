#ifndef STM_H
#define STM_H

#include <tuple>
#include <iostream>

#include "context.h"
#include "stm_runtime.h"
#include "stm_free.h"
#include "stm_bind.h"
#include "tvar.h"

namespace stm
{

// TODO: optimal random time backoff
// TODO: notifications instead of time wait

template <typename A>
using STML = free::STML<A>;

template <typename A>
A atomically(Context& context, const STML<A>& stml)
{
    return runSTM(context, stml);
}

template <typename A, typename B>
STML<B> bind(const STML<A> ma, const free::ArrowFunc<A, B>& f)
{
    return free::bindFree(ma, f);
}

const auto newTVar = [](const auto& val, const std::string& name = "")
{
    return free::newTVar(val, name);
};

template <typename A>
TVar<A> newTVarIO(Context& context, const A& val, const std::string& name = "")
{
    return atomically(context, free::newTVar<A>(val, name));
}

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

template <typename A>
const STML<fp::Unit>
modifyTVar(const TVar<A>& tvar, const std::function<A(A)>& f)
{
    auto m = readTVar(tvar);
    return bind<A, fp::Unit>(m, [=](const A& i)
    {
        return writeTVar(tvar, f(i));
    });
}

template <typename A>
const STML<A>
modifyTVarRet(const TVar<A>& tvar, const std::function<A(A)>& f)
{
    auto m = readTVar(tvar);
    return bind<A, fp::Unit>(m, [=](const A& i)
    {
        auto newResult = f(i);
        return sequence(writeTVar(tvar, newResult), pure(newResult));
    });
}

const auto modifyTVarT = [](const auto& tvar)
{
    return [&](const auto& f)
    {
        auto m = free::readTVar(tvar);
        return bind(m, [=](const auto& val)
        {
            return free::writeTVar(tvar, f(val));
        });
    };
};

template <typename A>
STML<A> retry(const A&)
{
    return free::retry<A>();
}

const auto retryF = [](const auto&)
{
    return free::retry<fp::Unit>();
};

const STML<fp::Unit> mRetry = free::retry<fp::Unit>();

const auto pure = [](const auto& a)
{
    return free::pureF(a);
};

template <typename A>
STML<fp::Unit> voided(const STML<A>& ma)
{
    return bind<A, fp::Unit>(ma, [](const A&) { return pure(fp::unit); });
}

template <typename A, typename B, typename Ret>
STML<Ret> both(const STML<A>& ma,
               const STML<B>& mb,
               const std::function<Ret(A, B)>& f)
{
    return bind<A, Ret>(ma, [=](const A& a){
        return bind<B, Ret>(mb, [=](const B& b){
            return pure(f(a, b));
        });
    });
}

template <typename A, typename B>
STML<fp::Unit> bothVoided(const STML<A>& ma,
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
    return both<A, B>(bind<TVar<A>, A>(ma, readTVar),
                      bind<TVar<B>, B>(mb, readTVar),
                      f);
}

template <typename A, typename B>
STML<B> sequence(const STML<A> ma, const STML<B>& mb)
{
    return both<A, B, B>(ma, mb, [](const A&, const B& b)
    {
        return b;
    });
}

template <typename A, typename B>
STML<B> ifThenElse(const STML<A>& m,
                   const STML<B>& mOnTrue,
                   const STML<B>& mOnFalse,
                   const std::function<bool(A)>& condF)
{
    return bind<A, B>(m, [=](const A& a) {
//        std::cout << "conditional cond: " << condF(a) << std::endl;
        return condF(a) ? mOnTrue : mOnFalse;
    });
}

template <typename B>
STML<B> ifThenElse(const STML<bool>& m,
                   const STML<B>& mOnTrue,
                   const STML<B>& mOnFalse)
{
    return ifThenElse<bool, B>(m, mOnTrue, mOnFalse, fp::id);
}

// Use these combinators with care. Prefer ifThenElse instead of both when and unless.
template <typename B>
STML<fp::Unit> when(const STML<bool>& ma, const STML<B>& mb)
{
    return bind<bool, fp::Unit>(ma, [=](bool cond) {
//        std::cout << "when cond: " << cond << std::endl;
        return cond
                ? voided<B>(mb)
                : pure(fp::unit);
    });
}

template <typename B>
STML<fp::Unit> unless(const STML<bool>& ma, const STML<B>& mb)
{
    return bind<bool, fp::Unit>(ma, [=](bool cond) {
//        std::cout << "unless cond: " << cond << std::endl;
        return cond
                ? pure(fp::unit)
                : voided<B>(mb);
    });
}

} // namespace stm

#endif // STM_H
