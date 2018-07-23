#ifndef STM_TYPES_H
#define STM_TYPES_H

#include <functional>
#include <variant>
#include <any>

#include "tvar.h"

namespace stm
{

// Short definitions
using Any = std::any;
using TVarAny = TVar<std::any>;

struct Unit
{
};

const Unit unit = {};

// TODO: && and std::forward??
const auto id = [](const auto& val) { return val; };

} // namespace stm

#endif // STM_TYPES_H
