#ifndef FREE_H
#define FREE_H

#include <variant>
#include <any>

// https://toby-allsopp.github.io/2016/10/12/free-monads-in-cpp.html

namespace fp
{

template <template <typename> class F, typename A>
struct PureF
{
    A ret;
};

// Forward declaration
template <template <typename> class F, typename A>
struct BindF;

template <template <typename> class F, typename A>
struct Free
{
    using ContainedType = A;
    using ReturnType    = PureF<F, A>;

    std::variant<PureF<F, A>, BindF<F, A>> v;
};

template <template <typename> class F, typename A>
struct BindF
{
    F<Free<F, A>> x;
};

template <template <typename> class F, typename A>
Free<F, A> make_pure(const A& x) {
//    PureF<F, A> p;
//    p.ret = x;
//    Free<F, A> res;
//    res.v = p;
//    return res;

    return {PureF<F, A>{x}};
}

// This one is for when you know the resulting Free type but not the template
// arguments.
template <typename FA>
FA make_pure(const typename FA::ContainedType& x) {
  return {typename FA::ReturnType{x}};
}

template <template <typename> class F, typename A>
Free<F, A> make_bind(const F<Free<F, A>>& x) {
  return {BindF<F, A>{x}};
}


template<typename A>
struct Wrapper : Free<F, A> {};

template <template <typename> class Wrapper>
  struct FunctorImpl {

    // The visitor struct can't be defined inside the fmap function because it
    // contains member function templates, which we use to get the compiler to
    // tell us what the template F is in the Free<F, A> inside the wrapper.
    template <typename A, typename Fn>
    struct Visitor {
      Fn& fun;

      template <template <typename> class F>
      auto operator()(const PureF<F, A>& r) const {
        return make_return<F>(fun(r.a));
      }

      template <template <typename> class F>
      auto operator()(const BindF<F, A>& b) const {
        using Functor::fmap;
        return make_bind(
                    fmap([&](const auto& f) { return fmap(fun, f); }, b.x));
      }

    };

    // fmap :: (a -> b) -> Free f a -> Free f b
    template <typename A, typename Fn>
    static Wrapper<std::result_of_t<Fn(A)>> fmap(Fn&& fun, const Wrapper<A>& f) {
      return boost::apply_visitor(Visitor<A, Fn>{fun}, f.v);
    }
  };

} // namespace fp

#endif // FREE_H
