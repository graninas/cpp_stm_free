#ifndef STM_CHURCH_INTERPRETER_H
#define STM_CHURCH_INTERPRETER_H

#include <functional>
#include <any>
#include <variant>
#include <iostream>

#include "stm_free_church.h"
#include "stm_church.h"
#include "context.h"

namespace stm
{
namespace church
{

//// forward declaration;
template <typename A>
struct StmfVisitor;

template <typename A>
RunResult<A> runSTMF(
        AtomicRuntime& runtime,
        const free::STMF<A>& stmf)
{
    StmfVisitor<A> visitor(runtime);
    std::visit(visitor, stmf);
    return visitor.result;
}

//type f :~> g = forall a. f a -> g a

//foldMap :: Monad g => (forall a. f a -> g a) -> F f a -> g a

//foldMap :: Monad g => f :~> g -> F f :~> g
//foldMap f (F k) = k return (join . f)

template <typename A>
RunResult<A> runSTML(AtomicRuntime& runtime,
                     const STML<A>& stml)
{
    std::function<free::Any(A)>
            pureAny = [](const A& a) { return a; }; // cast to any

    std::function<free::Any(free::STMF<free::Any>)> g
            = [&](const free::STMF<free::Any>& stmf)
    {
        auto runnerResult = runSTMF(runtime, stmf);
        if (runnerResult.retry)
        {
            throw std::runtime_error("Retry");
        }
        if (!runnerResult.result.has_value())
        {
            throw std::runtime_error("No result.");
        }
        return runnerResult.result.value();
    };

    A result;
    try
    {
        free::Any anyResult = stml.runF(pureAny, g);
        result = std::any_cast<A>(anyResult);
    }
    catch(std::runtime_error err)
    {
        if (strcmp(err.what(), "Retry") == 0)
        {
            return RunResult<A> { true, std::nullopt };
        }
        throw err;
    }
    return RunResult<A> { false, result };
}

template <typename Ret>
struct StmfVisitor
{
    AtomicRuntime& _runtime;

    StmfVisitor(AtomicRuntime& runtime)
        : _runtime(runtime)
    {
    }

    RunResult<Ret> result;

    template <typename A>
    void operator()(const free::NewTVar<A, Ret>& f)
    {
        auto tvarId = _runtime.newGUID();

//        std::cout << "<" << _runtime.getUStamp() << "> NewTVar. GUID: " << tvarId << ", name: " << f.name << std::endl;

        TVarHandle tvarHandle { _runtime.getUStamp(), f.val, true };
        _runtime.addTVarHandle(tvarId, tvarHandle);
        free::TVarAny tvar { f.name, tvarId };
        result = runSTML<Ret>(_runtime, f.next(tvar));
    }

    template <typename A>
    void operator()(const free::ReadTVar<A, Ret>& f)
    {
//        std::cout << "<" << _runtime.getUStamp() << "> ReadTVar. GUID: " << f.tvar.id << ", name: " << f.tvar.name << std::endl;

        TVarHandle tvarHandle = _runtime.getTVarHandle(f.tvar.id);
        result = runSTML<Ret>(_runtime, f.next(tvarHandle.data));
    }

    template <typename A>
    void operator()(const free::WriteTVar<A, Ret>& f)
    {
//        std::cout << "<" << _runtime.getUStamp() << "> WriteTVar. GUID: " << f.tvar.id << ", name: " << f.tvar.name << std::endl;

        _runtime.setTVarHandleData(f.tvar.id, f.val);
        result = runSTML<Ret>(_runtime, f.next(unit));
    }

    template <typename A>
    void operator()(const free::Retry<A, Ret>&)
    {
//        std::cout << "<" << _runtime.getUStamp() << "> Retry." << std::endl;

        result = { true, std::nullopt };
    }
};

} // namespace church
} // namespace stm

#endif // STM_CHURCH_INTERPRETER_H
