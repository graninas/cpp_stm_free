#ifndef TVAR_H
#define TVAR_H

#include <string>
#include <functional>

#include "stupid_guid.h"

namespace stm
{

using TVarId = utils::GUID;

template <typename T>
struct TVar
{
    std::string name;
    TVarId id;
};

} // namespace stm

#endif // TVAR_H
