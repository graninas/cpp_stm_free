#ifndef STM_H
#define STM_H

#include "context.h"
#include "stm_runtime.h"

#include "stm_free.h"
#include "stm_bind.h"

namespace stm
{

template <typename A>
A atomically(Context& context, const STML<A>& stml)
{
    return runSTM(context, stml);
}

} // namespace stm

#endif // STM_H
