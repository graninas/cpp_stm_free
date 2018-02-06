#include <QString>
#include <QtTest>

#include <stm.h>

#include "common.h"
#include "fork.h"

using namespace sample;

class STMTest : public QObject
{
    Q_OBJECT

public:
    STMTest();

private Q_SLOTS:

    void stmTest();
};

STMTest::STMTest()
{
}

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

typedef stm::TVar<Fork> TFork;

const std::function<void(TFork, Fork)> setTaken =
  [&](const TFork& tFork, const Fork& fork)
  {
    stm::writeTVar(tFork, {fork.name, ForkState::Taken});
  };


// https://gist.github.com/dpwright/6474401

const std::function<stm::STM<Unit>(stm::TVar<Fork>)> takeFork =
    [](const stm::TVar<Fork>& tFork)
{
    stm::readTVar(tFork)                >>=

       [](auto fork) {
            DO(setTaken(tFork, fork),
               RETURN(unit)
               )
            )
       };
};

//const std::function<stm::STM<Unit>(stm::TVar<Fork>)> takeFork =
//    [](const stm::TVar<Fork>& tFork)
//{
//    return
//        DO(stm::readTVar(tFork),
//           WITH(fork,
//                DO(setTaken(tFork, fork),
//                   RETURN(unit)
//                   )
//                )
//           );
//};

//const std::function<stm::STM<Unit>(stm::TVar<Fork>)> takeFork =
//    [](const stm::TVar<Fork>& tFork)
//{
//    auto fork = stm::readTVar(tFork);
//    auto r2   = stm::bind(
//                    r1,
//                    [](const Fork& fork) { return stm::writeTVar(tFork, {fork.name, ForkState::Taken}); }
//                );
//    return fp::unit;
//};

//const std::function<stm::STM<Unit>(stm::TVar<Fork>)> takeFork =
//    [](const stm::TVar<Fork>& tFork)
//{
//    return
//       stm::bindVoid(
//            stm::readTVar(tFork),
//            [](const Fork& fork) { return stm::writeTVar(tFork, {fork.name, ForkState::Taken}); }
//        );
//};

//    auto fork = stm::readTVar(tFork);
//    switch (fork.state)
//    {
//        case ForkState::Taken:
//            stm::writeTVar(tFork, Fork(fork.name, ForkState::Free));
//        break;
//        case ForkState::Free:
//        break;
//    }
//    return fp::unit;

void STMTest::stmTest()
{
//    auto fork1 = stm::newTVarIO<Fork>();

//    stm::atomically(takeFork(fork1));
//    stm::atomically(takeFork(fork1));
}

QTEST_APPLESS_MAIN(STMTest)

#include "tst_stm.moc"

