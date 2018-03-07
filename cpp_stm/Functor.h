// https://github.com/toby-allsopp/free-monads-in-cpp

#pragma once

#include <utility>

/*!
  ### Functors ###

  A "Functor" is something very precise in category theory, but you can think of
  it as something that somehow contains values with some structure and allows
  you to transform the contained values in a way that preserves the structure.

  In Haskell, it is defined like so:

  ```haskell
  class Functor f where
    fmap :: (a -> b) -> f a -> f b
  ```

  This just says that a type _f_ is a Functor if it supports the `fmap`
  function, which takes:

  - a function that operates on each value of type _a_ contained in the Functor,
    returning a value of type _b_ and
  - a value of type _f a_, being a particular value of our Functor containing
    values of type _a_

  and produces a value of type _f b_, being a particular value o four Functor
  containing values of type _b_.

  In C++, we will define a Functor as any class template with a single type
  parameter for which a valid specialization of the `Functor` class template
  exists.

*/

namespace Functor {
  /*!
    The default specialization of `Functor` is undefined; this will cause a
    compile-time error (or substitution failure when used in certain template
    contexts) if it is used.

    The first template parameter is the class template to be considered a
    Functor. The second template parameter is there to enable partial
    specializations to use SFINAE-based techniques.
   */
  template <template <typename> class T, typename = void>
  struct Functor;

  /*!
    It is very useful to have a concise way to query, at compile-time, whether a
    given class template has a valid `Functor` specialization. This is a perfect
    application of "Concepts" but as we wish to remain compatible with C++14
    compilers we have to do it the old-fashioned way, i.e. using partial
    template specialization and SFINAE.
   */
  namespace detail {
/*!
  Note that MS Visual C++ doesn't properly support expression SFINAE as of
  VS2015 update 3, so we just assume everything is a Functor there.
*/
#ifdef _MSC_VER
    template <template <typename> class>
    using IsFunctorT = std::true_type;
#else
    /*!
      In the usual C++ template metaprogramming fashion, we define a class
      template with the default case and then use partial specialization to
      change it for specific cases. Another common idiom is the use of an extra,
      defaulted template parameter that partial specializations can use to
      SFINAE themselves away.

      The default case is that any given class template is not a Functor.
     */
    template <template <typename> class, typename = void>
    struct IsFunctorT : std::false_type {};

    /*!
      Then we essentially say that any class template `T` is a Functor if a call to
      `Functor<T>::fmap` compiles and has the correct result type.
     */
    struct dummy1 {};
    struct dummy2 {};

    template <template <typename> class T>
    struct IsFunctorT<T,
                      std::enable_if_t<std::is_same<
                          T<dummy2>,
                          decltype(Functor<T>::fmap(std::declval<dummy2(dummy1)>(),
                                                    std::declval<T<dummy1>>()))>::value>>
        : std::true_type {};
#endif
  }

  /*!
    Using the `IsFunctorT` class template is little bit inconvenient, so we hide
    it away in the `detail` namespace and instead expose a `constexpr` variable
    template. An example of the use of this is coming up in the definition of `fmap`,
    below.
   */
  template <template <typename> class T>
  constexpr bool IsFunctor = detail::IsFunctorT<T>::value;

  /*!
    To make using the `fmap` function on a Functor value more convenient, we
    define a free function that wraps it. Using this allows the Functor template
    argument to be deduced.
   */
  template <template <typename> class F,
            typename A,
            typename Fun,
            typename = std::enable_if_t<IsFunctor<F>>>
  F<std::result_of_t<Fun(A)>> fmap(Fun&& fun, const F<A>& f) {
    return Functor<F>::fmap(std::forward<Fun>(fun), f);
  }

  /*!
    It is useful for testing purposes to have a very basic Functor---we call
    this `NullFunctor`. It contains exactly zero values, each of the requisite
    type.
  */
  namespace Test {
    template <typename A>
    struct NullFunctor {};
  }
  template <>
  struct Functor<Test::NullFunctor> {
    template <typename F, typename A>
    static Test::NullFunctor<std::result_of_t<F(A)>> fmap(F, Test::NullFunctor<A>) {
      return {};
    }
  };
  static_assert(IsFunctor<Test::NullFunctor>, "NullFunctor must be a Functor");
}
