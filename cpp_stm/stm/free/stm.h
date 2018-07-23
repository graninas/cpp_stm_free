#ifndef STM_FREE_STM_H
#define STM_FREE_STM_H

#include "../types.h"
#include "../context.h"
#include "../impl/runtime.h"
#include "stml.h"
#include "bind.h"
#include "interpreter.h"

namespace stm
{
namespace free
{

// STML generic monadic interface.

template <typename A, typename B>
STML<B> bind(const STML<A>& ma,
             const std::function<STML<B>(A)>& f)
{
    BindStmlVisitor<A, B> visitor(f);
    std::visit(visitor, ma.stml);
    return visitor.result;
}

template <typename A>
STML<A> join(const STML<STML<A>>& mma)
{
    return bind<STML<A>, A>(mma, [](const STML<A>& ma) { return ma; });
}

template <typename A, template <typename, typename> class Method>
static STML<A> wrap(const Method<Any, STML<A>>& method)
{
    return { FreeF<A> { stmf::STMF<STML<A>> { method } } };
}

template <typename A>
STML<A> pure(const A& a)
{
    return { PureF<A>{ a } };
}

template <typename A>
STML<A> retry()
{
    return wrap(stmf::RetryA<STML<A>> {});
}

template <typename A>
STML<TVar<A>> newTVar(
        const A& val,
        const std::string& name = "")
{
    auto r = stmf::NewTVar<A, STML<TVar<A>>>::toAny(
                val,
                name,
                [](const TVar<A>& tvar)
                    { return pure<TVar<A>>(tvar); }
                );
    return wrap(r);
}

template <typename A>
STML<A> readTVar(const TVar<A>& tvar)
{
    auto r = stmf::ReadTVar<A, STML<A>>::toAny(
                tvar,
                [](const A& a)
                    { return pure<A>(a); }
                );
    return wrap(r);
}

template <typename A>
STML<Unit> writeTVar(
        const TVar<A>& tvar,
        const A& val)
{
    auto r = stmf::WriteTVar<A, STML<Unit>>::toAny(
                tvar,
                val,
                [](const Unit&)
                    { return pure<Unit>(unit); }
                );
    return wrap(r);
}

template <typename A>
STML<Unit> modifyTVar(
        const TVar<A>& tvar,
        const std::function<A(A)>& f)
{
    return bind<A, Unit>(readTVar(tvar), [=](const A& val)
    {
        return writeTVar(tvar, f(val));
    });
}

/// STML evaluation

template <typename A>
A atomically(Context& context,
             const STML<A>& stml)
{
    RunnerFunc<A> runner = [&](AtomicRuntime& runtime)
    {
        return runSTML<A, StmlVisitor>(runtime, stml);
    };

    return runSTM<A>(context, runner);
}

// Special version of newTVar
template <typename A>
TVar<A> newTVarIO(Context& context,
                  const A& val,
                  const std::string& name = "")
{
    return atomically(context, newTVar<A>(val, name));
}

// Special version of readTVar. Can be possibly optimized to not to wait for conflicts.
template <typename A>
A readTVarIO(Context& context,
             const TVar<A>& tvar)
{
    return atomically(context, readTVar<A>(tvar));
}

/// Combinators

// Monadic values and lambdas (type inference can be better with them).

const STML<Unit> mRetry = retry<Unit>();

const STML<Unit> mUnit = pure<Unit>(unit);

const auto mPure = [](const auto& val)
{
    return pure(val);
};

const auto mNewTVar = [](const auto& val)
{
    return newTVar(val);
};

const auto mReadTVar = [](const auto& tvar)
{
    return readTVar(tvar);
};

const auto mWriteTVarT = [](const auto& tvar)
{
    return [=](const auto& val)
    {
        return writeTVar(tvar, val);
    };
};

const auto mWriteTVarV = [](const auto& val)
{
    return [=](const auto& tvar)
    {
        return writeTVar(tvar, val);
    };
};

// Generic combinators

// TODO: generic with using varargs and variadic templates

// Alias for `bind`.
template <typename A, typename B>
STML<B> with(const STML<A>& ma,
             const std::function<STML<B>(A)>& f)
{
    return bind<A, B>(ma, f);
}

template <typename A, typename B>
STML<B> with(const STML<A>& ma,
             const std::function<B(A)>& f)
{
    return bind<A, B>(ma, [=](const A& a)
    {
        return pure(f(a));
    });
}

template <typename A, typename B, typename C>
STML<C> both(const STML<A>& ma,
             const STML<B>& mb,
             const std::function<STML<C>(A, B)>& f)
{
    return bind<A, C>(ma, [=](const A& a){
        return bind<B, C>(mb, [=](const B& b){
            return f(a, b);
        });
    });
}

template <typename A, typename B, typename C>
STML<C> both(const STML<A>& ma,
             const STML<B>& mb,
             const std::function<C(A, B)>& f)
{
    return both<A, B, C>(ma, mb, [=](const A& a, const B& b) { return pure(f(a, b)); });
}

template <typename A, typename B>
STML<Unit> bothVoided(const STML<A>& ma,
                      const STML<B>& mb)
{
    return both<A, B, Unit>(ma, mb, [](const A&, const B&)
    {
        return unit;
    });
}

// TODO: rename it (`andThen`?)
// TODO: make sequence as in Haskell
template <typename A, typename B>
STML<B> sequence(
        const STML<A>& ma,
        const STML<B>& mb)
{
    return both<A, B, B>(ma, mb, [](const A&, const B& b)
    {
        return b;
    });
}

template <typename A>
STML<Unit> voided(const STML<A>& ma)
{
    return sequence<A, Unit>(ma, pure<Unit>(unit));
}

template <typename A, typename B>
STML<B> ifThenElse(const STML<A>& ma,
                   const STML<B>& mOnTrue,
                   const STML<B>& mOnFalse,
                   const std::function<bool(A)>& condF)
{
    return bind<A, B>(ma, [=](const A& a) {
        return condF(a) ? mOnTrue : mOnFalse;
    });
}

template <typename B>
STML<B> ifThenElse(const STML<bool>& mCond,
                   const STML<B>& mOnTrue,
                   const STML<B>& mOnFalse)
{
    return ifThenElse<bool, B>(mCond, mOnTrue, mOnFalse, id);
}

// Use `when` and `unless` combinators with care. Prefer ifThenElse instead.
// Reason: it's possible to evaluate some internal transaction several times by a mistake.
template <typename A>
STML<Unit> when(const STML<bool>& mCond,
                const STML<A>& ma)
{
    return ifThenElse<Unit>(mCond,
                            voided<A>(ma),
                            pure<Unit>(unit));
}

template <typename A>
STML<Unit> unless(const STML<bool>& mCond,
                  const STML<A>& ma)
{
    return ifThenElse<Unit>(mCond,
                            pure<Unit>(unit),
                            voided<A>(ma));
}

// Additional TVar combinators

template <typename A, typename B>
STML<B> withTVar(const STML<TVar<A>>& ma,
                 const std::function<STML<B>(A)>& f)
{
    return bind<A, B>(bind<TVar<A>, A>(ma, mReadTVar), f);
}

template <typename A, typename B>
STML<B> withTVar(const TVar<A>& tvar,
                 const std::function<STML<B>(A)>& f)
{
    return bind<A, B>(readTVar<A>(tvar), f);
}

template <typename A, typename B>
STML<B> withTVar(const TVar<A>& tvar,
                 const std::function<B(A)>& f)
{
    return bind<A, B>(readTVar(tvar),
                      [=](const A& a) { return pure(f(a)); });
}

template <typename A, typename B>
STML<Unit> whenTVar(const TVar<A>& tvar,
                    const std::function<bool(A)>& tvarCond,
                    const STML<B>& mb)
{
    return withTVar<A, Unit>(tvar, [=](const A& a)
    {
        return when<B>(pure(tvarCond(a)), mb);
    });
}

// TODO: replace by var args.

template <typename A, typename B, typename C>
STML<C> withTVars(const STML<TVar<A>>& ma,
                  const STML<TVar<B>>& mb,
                  const std::function<STML<C>(A, B)>& f)
{
    return both<A, B, C>(bind<TVar<A>, A>(ma, mReadTVar),
                         bind<TVar<B>, B>(mb, mReadTVar),
                         f);
}

template <typename A, typename B, typename C>
STML<C> withTVars(const STML<TVar<A>>& ma,
                  const STML<TVar<B>>& mb,
                  const std::function<C(A, B)>& f)
{
    return both<A, B, C>(bind<TVar<A>, A>(ma, mReadTVar),
                         bind<TVar<B>, B>(mb, mReadTVar),
                         f);
}

template <typename A, typename B, typename C>
STML<C> withTVars(const TVar<A>& tvar1,
                  const TVar<B>& tvar2,
                  const std::function<C(A, B)>& f)
{
    return both<A, B, C>(readTVar(tvar1),
                         readTVar(tvar2),
                         f);
}

template <typename A, typename B, typename C>
STML<C> withTVars(const TVar<A>& tvar1,
                  const TVar<B>& tvar2,
                  const std::function<STML<C>(A, B)>& f)
{
    return both<A, B, C>(readTVar(tvar1),
                         readTVar(tvar2),
                         f);
}

template <typename A>
STML<Unit> modifyTVarCurried(const TVar<A>& tvar)
{
    return [=](const auto& f)
    {
        return modifyTVar<A>(tvar, f);
    };
}

template <typename A>
STML<A> modifyTVarRet(const TVar<A>& tvar,
                      const std::function<A(A)>& f)
{
    return sequence<Unit, A>(modifyTVar<A>(tvar, f), readTVar<A>(tvar));
}

template <typename A>
STML<A> writeTVarRet(const TVar<A>& tvar,
                     const A& a)
{
    return sequence<Unit, A>(writeTVar(tvar, a), readTVar(tvar));
}

// Optional-aware functions

template <typename A, typename B>
STML<B> withOptional(const STML<std::optional<A>>& opt,
                     const STML<B>& onNullOpt,
                     const std::function<STML<B>(A)>& f)
{
    return bind<std::optional<A>, B>(opt, [=](const std::optional<A>& optA)
    {
        return optA.has_value() ? f(optA.value()) : onNullOpt;
    });
}

template <typename A, typename B>
STML<std::optional<B>> tryTVar(const TVar<A>& tvar,
                               const std::function<bool(A)>& tvarCond,
                               const STML<std::optional<B>>& mb)
{
    return withTVar<A, std::optional<B>>(tvar, [=](const A& a)
    {
        return tvarCond(a)
                ? mb
                : pure<std::optional<B>>(std::nullopt);
    });
}

template <typename A, typename B>
STML<std::optional<B>> bindOptional(
        const STML<std::optional<A>>& opt,
        const std::function<STML<std::optional<B>>(A)>& f)
{
    return withOptional<A, std::optional<B>>(opt, pure<std::optional<B>>(std::nullopt), f);
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


} // namespace free
} // namespace stm

#endif // STM_FREE_STM_H
