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

//template <typename A>
//STML<TVar<A>>
//    newTVarT(const A& val)
//{
//    STML<TVarAny> n = newTVar(val);
//    ArrowFunc<TVarAny, TVar<A>> f = [](const TVarAny& tvar)
//    {
//        return coerceTVarAny<TVar<A>>(tvar);
//    };

//    STML<TVar<A>> r = bind<TVarAny, TVar<A>>(n, f);
//    return r;
//}


} // namespace stm

#endif // STM_H
