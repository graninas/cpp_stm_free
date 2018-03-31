#ifndef STM_H
#define STM_H

#include <tuple>

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

template <typename A, typename B>
STML<B> bind(const STML<A> ma, const free::ArrowFunc<A, B>& f)
{
    return free::bindFree(ma, f);
}

const auto newTVar = [](const auto& val)
{
    return free::newTVar(val);
};

template <typename A>
TVar<A> newTVarIO(Context& context, const A& val)
{
    return atomically(context, free::newTVar<A>(val));
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

const auto modifyTVar = [](const auto& tvar, const auto& f)
{
    auto m = free::readTVar(tvar);
    return bind(m, [=](const auto& val)
    {
        return writeTVar(tvar, f(val));
    });
};

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

const auto retry = [](const auto&)
{
    return free::retry<fp::Unit>();
};

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

//template <typename A>
//STML<std::tuple<A>> readMany(const TVar<A>& tvar)
//{
//    STML<A> ma = readTVar(tvar);
//    return stm::bind<A, std::tuple<A>>(ma, [](const A& a)
//    {
//        return pure(std::make_tuple(a));
//    });
//}

//template <typename A, typename... Tail>
//STML<std::tuple<Tail>> readMany(const TVar<A>& tvar, Tail... tailTVars)
//{
//    STML<std::tuple<Tail...> mTail = readMany<Tail...>(tailTVars);
//    return stm::bind(mTail, [=](const std::tuple<Tail...>& tail)
//    {
//        STML<A> ma = readTVar(tvar);
//        return stm::bind<A, std::tuple<Tail...>>(ma, [=](const A& a)
//        {
//            return stm::pure(std::make_tuple(a, std::tie(tail)));
//        });
//    });
//}

template <typename A, typename B>
STML<B> sequence(const STML<A> ma, const STML<B>& mb)
{
    return both<A, B, B>(ma, mb, [](const A&, const B& b)
    {
        return b;
    });
}

const auto when = [](const STML<bool>& ma, const auto& mb)
{
    return bind<bool, fp::Unit>(ma, [=](bool cond) {
        return cond
                ? voided(mb)
                : pure(fp::unit);
    });
};

const auto unless = [](const STML<bool>& ma, const auto& mb)
{
    return bind<bool, fp::Unit>(ma, [=](bool cond) {
        return cond
                ? pure(fp::unit)
                : voided(mb);
    });
};

} // namespace stm

#endif // STM_H
