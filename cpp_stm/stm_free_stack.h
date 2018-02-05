#ifndef STM_FREE_STACK_H
#define STM_FREE_STACK_H

#include <functional>

namespace stm
{

template <typename ReadTVar, typename RTVar>
struct STML
{
    ReadTVar<RTVar> m_readTVarF;
};

} // namespace stm

#endif // STM_FREE_STACK_H
