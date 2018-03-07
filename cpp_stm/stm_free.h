#ifndef STM_FREE_H
#define STM_FREE_H

#include <functional>
#include <any>
#include <variant>

#include <unit.h>
#include <identity.h>

#include "free.h"

#include "tvar.h"
//#include "stm_free_stack.h"

namespace stm
{

// STM Free

template <typename Next>
struct NewTVarA
{
    std::any val;
    std::function<Next(TVar<std::any>)> next;
};

NewTVarA<TVar<std::any>>
    newTVarA(const std::any& val)
{
    NewTVarA<TVar<std::any>> f;
    f.val = val;
    f.next = fp::id;
    return f;
}

template <typename Next>
struct ReadTVarA
{
    TVar<std::any> tvar;
    std::function<Next(std::any)> next;
};

ReadTVarA<std::any>
    readTVarA(const TVar<std::any>& tvar)
{
    ReadTVarA<std::any> f;
    f.tvar = tvar;
    f.next = fp::id;
    return f;
}

template <typename Next>
struct WriteTVarA
{
    TVar<std::any> tvar;
    std::any val;
    Next next;
};

WriteTVarA<fp::Unit>
    writeTVarA(const TVar<std::any>& tvar, const std::any& val)
{
    WriteTVarA<fp::Unit> f;
    f.tvar = tvar;
    f.val  = val;
    f.next = fp::unit;
    return f;
}

template <class Next>
  using STMF = std::variant<NewTVarA<Next>, ReadTVarA<Next>, WriteTVarA<Next>>;

template <class Next>
    using STML = fp::FreeT<STMF, Next>;


// newTVar :: a -> Free f (TVar a)
// writeTVar :: TVar a -> a -> Free f Unit
// readTVar :: TVar a -> Free f a



} // namespace stm

#endif // STM_FREE_H

