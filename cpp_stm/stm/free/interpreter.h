#ifndef STM_FREE_INTERPRETER_H
#define STM_FREE_INTERPRETER_H

#include "stml.h"
#include "../context.h"

namespace stm
{
namespace free
{

// Forward declaration
template <typename Ret>
struct StmlVisitor;

template <typename Ret, template <typename> class Visitor>
RunResult<Ret> runSTML(AtomicRuntime& runtime, const STML<Ret>& stml)
{
    Visitor<Ret> visitor(runtime);
    std::visit(visitor, stml.stml);
    return visitor.result;
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
    void operator()(const stmf::NewTVar<A, STML<Ret>>& f)
    {
        auto tvarId = _runtime.newId();

//        std::cout << "<" << _runtime.getUStamp() << "> NewTVar. Id: " << tvarId << ", name: " << f.name << std::endl;

        TVarHandle tvarHandle { _runtime.getUStamp(), f.val, true };
        _runtime.addTVarHandle(tvarId, tvarHandle);
        TVarAny tvar { f.name, tvarId };
        result = runSTML<Ret, StmlVisitor>(_runtime, f.next(tvar));
    }

    template <typename A>
    void operator()(const stmf::ReadTVar<A, STML<Ret>>& f)
    {
//        std::cout << "<" << _runtime.getUStamp() << "> ReadTVar. Id: " << f.tvar.id << ", name: " << f.tvar.name << std::endl;

        TVarHandle tvarHandle = _runtime.getTVarHandle(f.tvar.id);
        result = runSTML<Ret, StmlVisitor>(_runtime, f.next(tvarHandle.data));
    }

    template <typename A>
    void operator()(const stmf::WriteTVar<A, STML<Ret>>& f)
    {
//        std::cout << "<" << _runtime.getUStamp() << "> WriteTVar. Id: " << f.tvar.id << ", name: " << f.tvar.name << std::endl;

        _runtime.setTVarHandleData(f.tvar.id, f.val);
        result = runSTML<Ret, StmlVisitor>(_runtime, f.next(unit));
    }

    template <typename A>
    void operator()(const stmf::Retry<A, STML<Ret>>&)
    {
//        std::cout << "<" << _runtime.getUStamp() << "> Retry." << std::endl;

        result = { true, std::nullopt };
    }
};

template <typename Ret>
struct StmlVisitor
{
    AtomicRuntime& _runtime;

    StmlVisitor(AtomicRuntime& runtime)
        : _runtime(runtime)
    {
    }

    RunResult<Ret> result;

    void operator()(const PureF<Ret>& p)
    {
//        std::cout << "\nPureF";
        result = { false, p.ret };
    }

    void operator()(const FreeF<Ret>& f)
    {
//        std::cout << "\nFreeF";
        StmfVisitor<Ret> visitor(_runtime);
        std::visit(visitor, f.stmf.stmf);
        result = visitor.result;
    }
};

} // namespace free
} // namespace stm

#endif // STM_FREE_INTERPRETER_H
