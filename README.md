C++ Software Transactional Memory library
=========================================

Working library for Software Transactional Memory that is built using several FP techniques and modern C++17.

- STM is monadic and combinatorial.
- It is very robust due to purely functional design.
- It is built on top of the custom Free monad.
- It operates by custom ADTs.
- It is usable despite it's experimental.

Requirements
------------

- GCC 7.2

Tutorials and samples
---------------------

 - [Tutorial: Software Transactional Memory in C++](https://gist.github.com/graninas/c7e0a603f3a22c7e85daa4599bf92525)
 - [Dining Philosophers Problem solved (variant 1)](https://github.com/graninas/cpp_philosophers_stm/blob/master/src/philosophers/philosophers_stm.h)
 - [Dining Philosophers Problem solved (variant 2)](https://github.com/graninas/cpp_philosophers_stm/blob/philosophers-variant2/src/philosophers/philosophers_stm.h)
 - [More samples](https://github.com/graninas/stm_samples)

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
