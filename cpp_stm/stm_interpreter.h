#ifndef STM_INTERPRETER_H
#define STM_INTERPRETER_H

#include <functional>
#include <any>
#include <variant>
#include <iostream>

#include <unit.h>
#include <identity.h>

#include "tvar.h"
#include "stm_free.h"
#include "context.h"

namespace stm
{

template <typename Ret, template <typename> class Visitor>
Ret runSTML(AtomicRuntime& runtime, const STML<Ret>& stml)
{
    Visitor<Ret> visitor(runtime);
    std::visit(visitor, stml.stml);
    return visitor.result;
}

// forward declaration;
template <typename Ret>
struct StmlVisitor;


template <typename Ret>
struct StmfVisitor
{
    AtomicRuntime& _runtime;

    StmfVisitor(AtomicRuntime& runtime)
        : _runtime(runtime)
    {
    }

    Ret result;

    void operator()(const NewTVarA<STML<Ret>>& f)
    {
        auto tvarId = _runtime.newGUID();

        std::cout << "\nNewTVarA. TVar GUID: " << tvarId;

        TVarHandle tvarHandle { _runtime.getUStamp(), f.val };
        _runtime.addTVarHandle(tvarId, tvarHandle);
        TVarAny tvar { tvarId };
        result = runSTML<Ret, StmlVisitor>(_runtime, f.next(tvar));
    }

    void operator()(const ReadTVarA<STML<Ret>>& f)
    {
        std::cout << "\nReadTVarA. TVar GUID: " << f.tvar.id;

        TVarHandle tvarHandle = _runtime.getTVarHandle(f.tvar.id);
        result = runSTML<Ret, StmlVisitor>(_runtime, f.next(tvarHandle.data));
    }

    void operator()(const WriteTVarA<STML<Ret>>& f)
    {
        std::cout << "\nWriteTVarA. TVar GUID: " << f.tvar.id;

        _runtime.setTVarHandleData(f.tvar.id, f.val);
        result = runSTML<Ret, StmlVisitor>(_runtime, f.next(fp::unit));
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

    Ret result;

    void operator()(const PureF<Ret>& p)
    {
        std::cout << "\nPureF";
        result = unPureF(p);
    }

    void operator()(const FreeF<Ret>& f)
    {
        std::cout << "\nFreeF";
        StmfVisitor<Ret> visitor(_runtime);
        std::visit(visitor, f.stmf.stmf);
        result = visitor.result;
    }
};

} // namespace stm

#endif // STM_INTERPRETER_H
