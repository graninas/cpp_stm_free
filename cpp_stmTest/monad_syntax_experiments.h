#ifndef MONAD_SYNTAX_EXPERIMENTS_H
#define MONAD_SYNTAX_EXPERIMENTS_H


//auto m1 = mb::just(amber);
//auto m2 = mb::bind(m1, lookupShadowVariator);
//auto m3 = mb::bind(m2, applyMovingVariator(amber, dir));
//auto m4 = mb::bind(m3, safeUpdateNearestPlace);
//return mb::maybe(m4, amber);

//const AmberTask tickOneAmberHour = [](const amber::Amber& amber)
//{
//    auto a1 = anyway(inflateShadowStorms, pure(amber));
//    //auto a2 = anyway(affectShadowStorms, a1);
//    auto a3 = anyway(shadowStabilization, a1);
//    auto a4 = anyway(tickWorldTime, a3);
//    return a4.amber;
//};


//const std::function<void(TFork, Fork)> setTaken =
//    [&](const TFork& tFork, const Fork& fork)
//    {
//        stm::writeTVar(tFork, {fork.name, ForkState::Taken});
//    };


//// https://gist.github.com/dpwright/6474401
//const std::function<stm::STM<fp::Unit>(stm::TVar<Fork>)> takeFork =
//    [](const stm::TVar<Fork>& tFork)
//{
//    return stm::readTVar(tFork)                                     >>= [&] (auto fork) {
//    return stm::writeTVar(tFork, Fork(fork.name, ForkState::Free));     };
//};

//#define SWITCH(var, cases) \
//    switch (var) \
//    { \
//        cases \
//    }

//#define CASE(pattern, act) \
//    case pattern: \
//        return act;

//const std::function<stm::STM<fp::Unit>(stm::TVar<Fork>)> takeFork =
//    [](const stm::TVar<Fork>& tFork)
//{
//    return stm::readTVar(tFork)                                   >>= [&] (auto fork) {
//           SWITCH (fork.state,
//               (
//                   CASE(ForkState::Taken, stm::writeTVar(tFork, Fork(fork.name, ForkState::Free)))
//                   CASE(ForkState::Free,  fp::unit)
//               );
//           )
//    };
//};

//const std::function<stm::STM<fp::Unit>(stm::TVar<Fork>)> takeFork =
//    [](const stm::TVar<Fork>& tFork)
//{
//    act_cont:
//    act

//    DO(act, accept_cont) ->

//    act >>= (
//        accept_cont
//    )

//    accept_cont:
//    WITH(var, act_cont) ->

//    [](auto const& var) {
//        act_cont
//    }



//    DO(stm::readTVar(tFork),
//       WITH(fork,
//            setTaken(tFork, fork);
//            )
//       );
//};

//const std::function<stm::STM<fp::Unit>(stm::TVar<Fork>)> takeFork =
//    [](const stm::TVar<Fork>& tFork)
//{
//    return
//        DO(stm::readTVar(tFork),
//           WITH(fork,
//                DO(setTaken(tFork, fork),
//                   RETURN(fp::unit)
//                   )
//                )
//           );
//};

//const std::function<stm::STM<fp::Unit>(stm::TVar<Fork>)> takeFork =
//    [](const stm::TVar<Fork>& tFork)
//{
//    auto fork = stm::readTVar(tFork);
//    return      stm::bind(r1, setTaken);
//};

//const std::function<stm::STM<fp::Unit>(stm::TVar<Fork>)> takeFork =
//    [](const stm::TVar<Fork>& tFork)
//{
//    return
//       stm::bindVoid(
//            stm::readTVar(tFork),
//            [](const Fork& fork) { return stm::writeTVar(tFork, {fork.name, ForkState::Taken}); }
//        );
//};

#endif // MONAD_SYNTAX_EXPERIMENTS_H
