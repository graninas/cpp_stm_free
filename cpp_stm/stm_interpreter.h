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
Ret runSTML(Context& context, const STML<Ret>& stml)
{
    Visitor<Ret> visitor(context);
    std::visit(visitor, stml.stml);
    return visitor.result;
}


template <typename Ret>
struct MockStmfVisitor
{
    Context& _context;

    MockStmfVisitor(Context& context)
        : _context(context)
    {
    }

    Ret result;

    void operator()(const NewTVarA<STML<Ret>>& f)
    {
        std::cout << "\nNewTVarA";
//        TVarAny tvar = _context.createNewTVar(f.val);
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
    Context& _context;

    MockFreeVisitor(Context& context)
        : _context(context)
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

        MockStmfVisitor<Ret> visitor(_context);
        std::visit(visitor, f.stmf.stmf);
    }

};

} // namespace stm

#endif // STM_INTERPRETER_H
