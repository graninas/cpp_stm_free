#ifndef STM_STMLR_H
#define STM_STMLR_H

#include "tvar.h"
#include "stm_free.h"

namespace stm {


template <typename Next, typename A>
struct CoerceTVarAny
{
    TVarAny tvar;
    std::function<Next(TVar<A>)> next;
};





template <typename A>
STML<TVar<A>>
    coerceTVarAny(const TVarAny& tvar)
{
    CoerceTVarAny<A, STML<TVar<A>>> n;
    n.tvar = tvar;
    n.next = [](const TVar<A>& tvar){
        return pureF(tvar);
    };
//    return wrap<STML<TVar<A>, CoerceTVarAny<A>>(n);
    return wrap(n);
}

}

#endif // STM_STMLR_H
