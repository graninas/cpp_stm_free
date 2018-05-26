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

Troubleshooting
---------------

- Pass tvars to closures by copy.
- Make `retry` satisfiable.

See also: Dining Philosophers
-----------------------------

The Dining Philosophers task is solved with this STM library. Check this project to see how to use STM.

https://github.com/graninas/cpp_philosophers_stm

See also: Amber
---------------

Showcase of functional declarative design in C++.

Game about Amber from The Chronicles of Amber.

https://github.com/graninas/Amber

See also: CMLife
----------------

CMLife - another showcase of functional declarative design in C++.

Parallel celullar automata built on top of comonads and std::future used as monad.

https://github.com/graninas/CMLife
