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

/// STML monadic interface

template <typename A>
using STML = free::STML<A>;

template <typename A, typename B>
STML<B> bind(const STML<A> ma,
             const std::function<STML<B>(A)>& f)
{
    free::BindStmlVisitor<A, B> visitor(f);
    std::visit(visitor, ma.stml);
    return visitor.result;
}

template <typename A>
STML<A> join(const STML<STML<A>> mma)
{
    return bind<STML<A>, A>(mma, [](const STML<A>& ma) { return ma; });
}

template <typename A>
STML<A> pure(const A& a)
{
    return { free::PureF<A>{ a } };
}

template <typename A>
STML<A> retry()
{
    return free::wrapA(free::RetryA<STML<A>> {});
}

template <typename A>
STML<TVar<A>> newTVar(const A& val,
                      const std::string& name = "")
{
    free::NewTVar<A, STML<TVar<A>>> n;
    n.val = val;
    n.name = name;
    n.next = [](const TVar<A>& tvar) {
        return free::pureF(tvar);
    };
    return free::wrapT(n);
}

template <typename A>
STML<A> readTVar(const TVar<A>& tvar)
{
    free::ReadTVar<A, STML<A>> n;
    n.tvar = tvar;
    n.next = [](const A& val) {
        return free::pureF(val);
    };
    return free::wrapT(n);
}

template <typename A>
STML<fp::Unit> writeTVar(const TVar<A>& tvar,
                         const A& val)
{
    free::WriteTVar<A, STML<fp::Unit>> n;
    n.tvar = tvar;
    n.val  = val;
    n.next = [](const fp::Unit& unit) {
        return free::pureF(unit);
    };
    return free::wrapT(n);
}

template <typename A>
STML<fp::Unit> modifyTVar(const TVar<A>& tvar,
                          const std::function<A(A)>& f)
{
    return bind<A, fp::Unit>(readTVar(tvar), [=](const A& val)
    {
        return writeTVar(tvar, f(val));
    });
}

/// STML evaluation

template <typename A>
A atomically(Context& context,
             const STML<A>& stml)
{
    return free::runSTM(context, stml);
}

// Special version of newTVarIO
template <typename A>
TVar<A> newTVarIO(Context& context,
                  const A& val,
                  const std::string& name = "")
{
    return atomically(context, newTVar<A>(val, name));
}

/// Combinators

// Monadic values and lambdas (type inference can be better with them).

const STML<fp::Unit> mRetry = retry<fp::Unit>();

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
    return [&](const auto& val)
    {
        return writeTVar(tvar, val);
    };
};

const auto mWriteTVarV = [](const auto& val)
{
    return [&](const auto& tvar)
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
STML<fp::Unit> bothVoided(const STML<A>& ma,
                          const STML<B>& mb)
{
    return both<A, B, fp::Unit>(ma, mb, [](const A&, const B&)
    {
        return fp::unit;
    });
}

// TODO: rename it (`andThen`?)
// TODO: make sequence as in Haskell
template <typename A, typename B>
STML<B> sequence(const STML<A> ma,
                 const STML<B>& mb)
{
    return both<A, B, B>(ma, mb, [](const A&, const B& b)
    {
        return b;
    });
}

template <typename A>
STML<fp::Unit> voided(const STML<A>& ma)
{
    return sequence<A, fp::Unit>(ma, pure(fp::unit));
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
    return ifThenElse<bool, B>(mCond, mOnTrue, mOnFalse, fp::id);
}

// Use `when` and `unless` combinators with care. Prefer ifThenElse instead.
// Reason: it's possible to evaluate some internal transaction several times by a mistake.
template <typename A>
STML<fp::Unit> when(const STML<bool>& mCond,
                    const STML<A>& ma)
{
    return ifThenElse<fp::Unit>(mCond, voided<A>(ma), pure(fp::unit));
}

template <typename A>
STML<fp::Unit> unless(const STML<bool>& mCond,
                      const STML<A>& ma)
{
    return ifThenElse<fp::Unit>(mCond, pure(fp::unit), voided<A>(ma));
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
STML<fp::Unit> modifyTVarCurried(const TVar<A>& tvar)
{
    return [&](const auto& f)
    {
        return modifyTVar<A>(tvar, f);
    };
}

template <typename A>
STML<A> modifyTVarRet(const TVar<A>& tvar,
                      const std::function<A(A)>& f)
{
    return sequence<fp::Unit, A>(modifyTVar<A>(tvar, f), readTVar<A>(tvar));
}

} // namespace stm

#endif // STM_H
