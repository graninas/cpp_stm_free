#include <QString>
#include <QtTest>

#include <variant>

#include "common.h"
#include "fork.h"
#include "stm_free.h"
#include "stm_interpreter.h"
#include "stupid_guid.h"

using namespace sample;

class STMTest : public QObject
{
    Q_OBJECT

public:
    STMTest();

private Q_SLOTS:

    void stupidGuidTest();

    void stmaConstructionTest();
    void stmfConstructionTest();
    void stmlConstructionTest();

    void visitorTest();



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

void STMTest::stupidGuidTest()
{
    // stupid test of stupid guid
    std::vector<utils::GUID> guids;

    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(1, 16);
    utils::Dice dice = std::bind ( distribution, generator );

    for (auto i = 0; i < 10; ++i)
    {
        utils::GUID newGUID = utils::newGUID(dice);
        QVERIFY(newGUID.size() == 32);
        for (std::vector<int>::size_type j = 0; j < guids.size(); ++j)
        {
            QVERIFY(guids[j] != newGUID);
        }
        guids.push_back(newGUID);
    }
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

    stm::STML<int> a1 = stm::pureF(10);
    stm::STML<stm::STMF<stm::TVar<std::any>>> a2 = stm::pureF(f1);
    stm::STML<stm::TVar<std::any>> a4 = stm::newTVar(10);
    stm::STML<std::any> a5 = stm::readTVar(fakeTVar);
    stm::STML<fp::Unit> a6 = stm::writeTVar(fakeTVar, 10);
}

void STMTest::visitorTest()
{
    std::any any = std::any(10);
    stm::TVar<std::any> fakeTVar;

    stm::STML<int>      a1 = stm::pureF(10);
    stm::STML<stm::TVar<std::any>> a2 = stm::newTVar(10);

    stm::Context context1;
    auto ustamp1 = context1.newGUID();
    stm::AtomicRuntime runtime1(context1, ustamp1);

    int result1 = stm::runSTML<int, stm::MockFreeVisitor>(runtime1, a1);
    QVERIFY(result1 == 10);

    stm::Context context2;
    auto ustamp2 = context2.newGUID();
    stm::AtomicRuntime runtime2(context2, ustamp2);
    stm::runSTML<stm::TVar<std::any>, stm::MockFreeVisitor>(runtime2, a2);
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

