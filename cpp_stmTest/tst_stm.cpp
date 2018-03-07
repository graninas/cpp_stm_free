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

    void stmaConstructionTest();
    void stmfConstructionTest();
    void stmlConstructionTest();



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

void STMTest::stmaConstructionTest()
{
    std::any any = std::any(10);
    stm::TVar<std::any> fakeTVar;

    stm::NewTVarA<stm::TVar<std::any>> constr1 = stm::newTVarA(any);
    stm::WriteTVarA<fp::Unit>          constr2 = stm::writeTVarA(fakeTVar, any);
    stm::ReadTVarA<std::any>           constr3 = stm::readTVarA(fakeTVar);
}

void STMTest::stmfConstructionTest()
{
    std::any any = std::any(10);
    stm::TVar<std::any> fakeTVar;

    stm::NewTVarA<stm::TVar<std::any>> constr1 = stm::newTVarA(any);
    stm::WriteTVarA<fp::Unit>          constr2 = stm::writeTVarA(fakeTVar, any);
    stm::ReadTVarA<std::any>           constr3 = stm::readTVarA(fakeTVar);

    stm::STMF<stm::TVar<std::any>> f1 { constr1 };
    stm::STMF<fp::Unit>            f2 { constr2 };
    stm::STMF<std::any>            f3 { constr3 };
}

void STMTest::stmlConstructionTest()
{
    std::any any = std::any(10);
    stm::TVar<std::any> fakeTVar;

    stm::NewTVarA<stm::TVar<std::any>> constr1 = stm::newTVarA(any);
    stm::WriteTVarA<fp::Unit>          constr2 = stm::writeTVarA(fakeTVar, any);
    stm::ReadTVarA<std::any>           constr3 = stm::readTVarA(fakeTVar);

    stm::STMF<stm::TVar<std::any>> f1 { constr1 };
    stm::STMF<fp::Unit>            f2 { constr2 };
    stm::STMF<std::any>            f3 { constr3 };

    auto a1 = stm::pureF(10);
    auto a2 = stm::pureF(f1);
    auto a3 = stm::newTVarX(10);
    auto a4 = stm::newTVar(10);
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

