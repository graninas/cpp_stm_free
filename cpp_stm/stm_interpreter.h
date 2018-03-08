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
struct MockFreeVisitor;


template <typename Ret>
struct MockStmfVisitor
{
    AtomicRuntime& _runtime;

    MockStmfVisitor(AtomicRuntime& runtime)
        : _runtime(runtime)
    {
    }

    Ret result;

    void operator()(const NewTVarA<STML<Ret>>& f)
    {
        std::cout << "\nNewTVarA";

        auto tvarId = _runtime.newGUID();
        TVarHandle tvarHandle { _runtime.getUStamp(), f.val };
        _runtime.addTVarHandle(tvarId, tvarHandle);
        TVarAny tvar { tvarId };

        auto nextStml = f.next(tvar);
        result = runSTML<Ret, stm::MockFreeVisitor>(_runtime, nextStml);
    }

    void operator()(const ReadTVarA<STML<Ret>>& f)
    {
        std::cout << "\nReadTVarA";
    }

    void operator()(const WriteTVarA<STML<Ret>>& f)
    {
        std::cout << "\nWriteTVarA";
    }
};

template <typename Ret>
struct MockFreeVisitor
{
    AtomicRuntime& _runtime;

    MockFreeVisitor(AtomicRuntime& runtime)
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

        MockStmfVisitor<Ret> visitor(_runtime);
        std::visit(visitor, f.stmf.stmf);
        result = visitor.result;
    }

};

} // namespace stm

#endif // STM_INTERPRETER_H
