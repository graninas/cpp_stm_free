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

    void stmcConstructionTest();
    void stmwConstructionTest();



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

void STMTest::stmcConstructionTest()
{
    stm::TVar<int> fakeTVar;

    stm::NewTVarC<int, stm::TVar<int>> constr1 = stm::newTVarC(10);
    stm::WriteTVarC<int, fp::Unit>     constr2 = stm::writeTVarC(fakeTVar, 10);
    stm::ReadTVarC<int, int>           constr3 = stm::readTVarC(fakeTVar);
}

void STMTest::stmwConstructionTest()
{
    stm::TVar<int> fakeTVar;


}


void STMTest::stmTest()
{
}


void STMTest::stmBindPureTest()
{
}

void STMTest::stmMapPureFreeTest()
{
}

void STMTest::stmMapNewTVarFreeTest()
{
}

void STMTest::stmPassFreeTest()
{
}

void STMTest::stmBindFreeTest()
{
}

void STMTest::stmUnFreeTest()
{
}

QTEST_APPLESS_MAIN(STMTest)

#include "tst_stm.moc"

