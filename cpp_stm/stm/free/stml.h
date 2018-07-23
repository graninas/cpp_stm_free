#ifndef STM_FREE_STML_H
#define STM_FREE_STML_H

#include "../tvar.h"
#include "../types.h"
#include "../stmf/stmf.h"

namespace stm
{
namespace free
{
// STM Free

// Forward declaration
template <typename A>
struct STML;

// Free methods

template <typename Ret>
struct PureF
{
    Ret ret;
};

template <typename Ret>
struct FreeF
{
    stmf::STMF<STML<Ret>> stmf;
};

// Recursive Free STML algebraic data type

template <typename A>
struct STML
{
    std::variant<PureF<A>, FreeF<A>> stml;
};

} // namespace free
} // namespace stm

#endif // STM_FREE_STML_H

