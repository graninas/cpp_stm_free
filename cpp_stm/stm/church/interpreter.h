#ifndef STM_CHURCH_INTERPRETER_H
#define STM_CHURCH_INTERPRETER_H

#include "stml.h"
#include "../context.h"
#include <cstring>

namespace stm
{
namespace church
{

// Forward declaration
template <typename Ret>
struct StmfVisitor;

template <typename A>
RunResult<A> runSTMF(
        AtomicRuntime& runtime,
        const stmf::STMF<A>& stmf)
{
    StmfVisitor<A> visitor(runtime);
    std::visit(visitor, stmf.stmf);
    return visitor.result;
}

template <typename A>
RunResult<A> runSTML(AtomicRuntime& runtime,
                     const STML<A>& stml)
{
    std::function<Any(A)>
            pureAny = [](const A& a) { return a; }; // cast to any

    std::function<Any(stmf::STMF<Any>)> g
            = [&](const stmf::STMF<Any>& stmf)
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
        Any anyResult = stml.runF(pureAny, g);
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
    void operator()(const stmf::NewTVar<A, Ret>& f)
    {
        auto tvarId = _runtime.newId();

//        std::cout << "<" << _runtime.getUStamp() << "> NewTVar. Id: " << tvarId << ", name: " << f.name << std::endl;

        TVarHandle tvarHandle { _runtime.getUStamp(), f.val, true };
        _runtime.addTVarHandle(tvarId, tvarHandle);
        TVarAny tvar { f.name, tvarId };
        result.retry = false;
        result.result = f.next(tvar);
    }

    template <typename A>
    void operator()(const stmf::ReadTVar<A, Ret>& f)
    {
//        std::cout << "<" << _runtime.getUStamp() << "> ReadTVar. Id: " << f.tvar.id << ", name: " << f.tvar.name << std::endl;

        TVarHandle tvarHandle = _runtime.getTVarHandle(f.tvar.id);
        result.retry = false;
        result.result = f.next(tvarHandle.data);
    }

    template <typename A>
    void operator()(const stmf::WriteTVar<A, Ret>& f)
    {
//        std::cout << "<" << _runtime.getUStamp() << "> WriteTVar. Id: " << f.tvar.id << ", name: " << f.tvar.name << std::endl;

        _runtime.setTVarHandleData(f.tvar.id, f.val);
        result.retry = false;
        result.result = f.next(unit);
    }

    template <typename A>
    void operator()(const stmf::Retry<A, Ret>&)
    {
//        std::cout << "<" << _runtime.getUStamp() << "> Retry." << std::endl;

        result = { true, std::nullopt };
    }
};

} // namespace church
} // namespace stm

#endif // STM_CHURCH_INTERPRETER_H
