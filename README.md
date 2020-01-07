C++ Software Transactional Memory library
=========================================

Working library for Software Transactional Memory that is built using several FP techniques and modern C++17.

- STM is monadic and combinatorial.
- It is very robust due to purely functional design.
- It is built on top of the custom Free monad.
- It operates by custom ADTs.
- It is usable despite it's experimental.

**Additional materials**
- [Tutorial: Software Transactional Memory in C++](https://gist.github.com/graninas/c7e0a603f3a22c7e85daa4599bf92525)
- [Dining Philosophers Problem solved (variant 1)](https://github.com/graninas/cpp_philosophers_stm/blob/master/src/philosophers/philosophers_stm.h)
- [Dining Philosophers Problem solved (variant 2)](https://github.com/graninas/cpp_philosophers_stm/blob/philosophers-variant2/src/philosophers/philosophers_stm.h)
- [More samples](https://github.com/graninas/stm_samples)
- [stm-free](https://github.com/graninas/stm-free) | Implementation in Haskell
- [cpp_parsec_free](https://github.com/graninas/cpp_parsec_free) | Implementation of Haskell's Parsec in C++ using the same Free Monad approach
- [Functional Approach To Software Transactional Memory in C++ (Talk, Rus)](https://www.youtube.com/watch?v=VHZPcz8HwZs) | [Slides (Eng)](https://docs.google.com/presentation/d/1_znOLZDKruKRNLA58TDlnXYQjTg9sXNJHPOLHfDTjeU/edit?usp=sharing) | My talk about this approach at C++ Russia 2018, Saint-Petersburg
- [Functional Programming in C++ and Concurrent Calculations (Talk, Rus)](https://www.youtube.com/watch?v=0v0T_Mny7EY) | [Slides (Eng)](https://docs.google.com/presentation/d/1Fuq1EwaB3tKkNckLau56WhO_VtbNEDJb-Kc6MpfBi0k/edit?usp=sharing) | My talk about this approach at LambdaNsk meetup, Novosibirsk, 2019

Requirements
------------

- GCC 7.2

Troubleshooting
---------------

- Pass tvars to closures by copy.
- Make `retry` satisfiable.

Examples
--------

The simplest possible usage is int counter increment from different threads.

Transaction:

```cpp
stm::STML<int> incrementCounter(const stm::TVar<int>& tCounter) {
    stm::STML<stm::Unit> modified =
            stm::modifyTVar<int>(tCounter, [](int i) { return i + 1; });

    return stm::bind<stm::Unit, int>(modified,
                     [&](const stm::Unit&){ return stm::readTVar<int>(tCounter); });
}
```

Evaluation:

```cpp
Context ctx;

stm::TVar<int> tCounter = stm::newTVarIO(ctx, 0);
int counter = stm::atomically(ctx, incrementCounter(tCounter));
std::cout << "Counter: " << counter << std::endl;
```

The `Dining Philosopher Problem` can be solved with STM elegantly. Here are the transactions for taking of forks:

```cpp
stm::STML<stm::Unit> takeFork(const TFork& tFork) {
    return stm::withTVar<Fork, stm::Unit>(tFork, [=](const Fork& fork) {
       if (fork.state == ForkState::Free) {
           return stm::writeTVar<Fork>(tFork, Fork { fork.name, ForkState::Taken });
       }
       else {
           return stm::retry<stm::Unit>();
       }
    });
}

stm::STML<stm::Unit> takeForks(const TForkPair& forks) {
    stm::STML<stm::Unit> lm = takeFork(forks.left);
    stm::STML<stm::Unit> rm = takeFork(forks.right);
    return stm::sequence(lm, rm);
}
```

Notice the `retry` combinator that marks some state illegal and makes the transaction to restart on case if fork was already taken.

To get more information, read the [tutorial](https://gist.github.com/graninas/c7e0a603f3a22c7e85daa4599bf92525).
