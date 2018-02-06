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
    void stmFreeTest();
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

void STMTest::stmFreeTest()
{
//    auto freeR1 = stm::newTVar(10);
//    auto freeR2 = stm::bindFree(freeR1, [](auto val){ return stm::pureFree(val); } );
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

