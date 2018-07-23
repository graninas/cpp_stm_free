#ifndef STM_FREE_STM_H
#define STM_FREE_STM_H

#include "../types.h"
#include "../context.h"
#include "../impl/runtime.h"
#include "stml.h"
#include "bind.h"
#include "interpreter.h"

namespace stm {
namespace free {

#include "../generic/stm.h"
#include "../generic/optional.h"

} // namespace free
} // namespace stm

#endif // STM_FREE_STM_H
