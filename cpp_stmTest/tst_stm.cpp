#include <QString>
#include <QtTest>

#include <stm.h>

#include "common.h"
#include "fork.h"

#include "stm_free.h"

using namespace sample;

class STMTest : public QObject
{
    Q_OBJECT

public:
    STMTest();

private Q_SLOTS:

    void stmTest();
    void stmMapPureFreeTest();
    void stmMapNewTVarFreeTest();
    void stmPassFreeTest();
    void stmBindPureTest();
    void stmBindFreeTest();
    void stmUnFreeTest();
};

STMTest::STMTest()
{
}

typedef stm::TVar<Fork> TFork;

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


void STMTest::stmBindPureTest()
{
    std::function<stm::Free<stm::Pure<int, fp::Unit>>(int)>
        f = [](int val){ return stm::pureFree(val + 10); };
    auto freeR1 = stm::pureFree(10);
    auto freeR2 = stm::bindFree(freeR1, f);
    auto result = stm::unFree(freeR2);
    QVERIFY(result == 20);
}

void STMTest::stmMapPureFreeTest()
{
    // Type deducing doesn't work on lambdas
    std::function<int(int)>         f1 = [](int x) { return x + 20; };
    std::function<std::string(int)> f2 = [](int)   { return "abc";  };

    auto freeR1 = stm::pureFree(10);
    auto freeR2 = stm::mapFree(f1, freeR1);
    auto freeR3 = stm::mapFree(f2, freeR2);

    auto result1 = stm::unFree(freeR2);
    auto result2 = stm::unFree(freeR3);
    QVERIFY(result1 == 30);
    QVERIFY(result2 == "abc");
}

void STMTest::stmMapNewTVarFreeTest()
{
    std::function<int(int)>         f1 = [](int x) { return x + 20; };
    std::function<std::string(int)> f2 = [](int)   { return "abc";  };

    stm::Free<stm::NewTVar<int, stm::TVar<int>>> freeR1 = stm::newTVar<int, stm::TVar<int>>(10);
//    auto freeR2 = stm::mapFree(f1, freeR1);
//    auto freeR3 = stm::mapFree(f2, freeR2);

//    auto result1 = stm::unFree(freeR2);
//    auto result2 = stm::unFree(freeR3);
//    QVERIFY(result1 == 30);
//    QVERIFY(result2 == 20);
}

void STMTest::stmPassFreeTest()
{
//    std::function<
//            stm::Free<stm::NewTVar<int,
//                                  stm::Free<stm::Pure<int, fp::Unit>, int>
//                                  >,
//                      fp::Unit>
//            (int)>
//        f = [](int val){ return stm::newTVar(val); };

//    auto freeR1 = stm::pureFree(10);
//    auto freeR2 = stm::bindFree(freeR1, f);

    auto freeR1 = stm::pureFree(10);
    stm::passFree(freeR1);
}

void STMTest::stmBindFreeTest()
{
//    std::function<
//            stm::Free<stm::NewTVar<int,
//                                  stm::Free<stm::Pure<int, fp::Unit>, int>
//                                  >,
//                      fp::Unit>
//            (int)>
//        f = [](int val){ return stm::newTVar(val); };

//    auto freeR1 = stm::pureFree(10);
//    auto freeR2 = stm::bindFree(freeR1, f);

    QVERIFY(false);
}

void STMTest::stmUnFreeTest()
{
    auto freeVal     = stm::pureFree(10);
    auto freeFreeVal = stm::pureFree(freeVal);

    auto val1 = stm::unFree(freeVal);
    auto val2 = stm::unFree(stm::unFree(freeFreeVal));

    QVERIFY(val1 == 10);
    QVERIFY(val2 == 10);
}

QTEST_APPLESS_MAIN(STMTest)

#include "tst_stm.moc"

