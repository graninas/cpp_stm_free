#ifndef FREE_H
#define FREE_H

#include <variant>
#include <any>

namespace fp
{

template <template <typename> class Algebra, typename Ret>
struct PureF
{
    Ret ret;
};

// Forward declaration
template <template <typename> class Algebra, typename Ret>
struct FreeF;

// Question: what to use: FreeT or Free?
template <template <typename> class Algebra, typename Ret>
    using FreeT = std::variant<PureF<Algebra, Ret>, FreeF<Algebra, Ret>>;

template <template <typename> class Algebra, typename Ret>
struct Free
{
    FreeT<Algebra, Ret> free;
};

template <template <typename> class Algebra, typename Ret>
struct FreeF
{
    Algebra<Free<Algebra, Ret>> method;
};


} // namespace fp

#endif // FREE_H
