#ifndef FREE_H
#define FREE_H

#include <variant>

namespace fp
{

//#define FreePureT Free<Pure<T, fp::Unit>>
//#define Fun(FromT, ToT) std::function<ToT(FromT)>

// forward declarations, experiment
template <typename Next>
struct NewTVar;

template <typename Next>
struct ReadTVar;

template <typename Next>
struct WriteTVar;

template <class Next>
  using STMT = std::variant<NewTVar<Next>, ReadTVar<Next>, WriteTVar<Next>>;


template <template <typename> class AlgebraMethod, typename Ret>
struct Pure
{
    Ret ret;
};

//data Free f a = Pure a | Free (f (Free f a))

template <template <typename> class AlgebraMethod, typename Ret>
struct Free
{
    AlgebraMethod<Ret> method;
};

//template <typename T>
//FreePureT pureFree(const T& ret)
//{
//    auto f = FreePureT();
//    f.method.ret = ret;
//    return f;
//}

} // namespace fp

#endif // FREE_H
