#ifndef STM_CHURCH_STML_H
#define STM_CHURCH_STML_H

#include "../tvar.h"
#include "../types.h"
#include "../stmf/stmf.h"
#include "../stmf/functor.h"

namespace stm
{
namespace church
{

// STM Free Church-encoded

template <typename A>
struct STML
{
    std::function<
        Any(
            std::function<Any(A)>,
            std::function<Any(stmf::STMF<Any>)>
        )> runF;

};

} // namespace church
} // namespace stm

#endif // STM_CHURCH_STML_H

