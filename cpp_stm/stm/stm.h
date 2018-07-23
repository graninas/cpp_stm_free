#ifndef STM_H
#define STM_H

#include "church/stm.h"
#include "free/stm.h"

namespace stm {

#ifdef STM_CHURCH
using namespace church;
#else
using namespace free;
#endif

} // namespace stm

#endif // STM_H
