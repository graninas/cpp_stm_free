#include <QString>
#include <QtTest>

#include <variant>
#include <iostream>

#include "common.h"
#include "stupid_guid.h"
#include "fork.h"
#include "stm_free.h"
#include "stm_bind.h"
#include "stm_interpreter.h"
#include "stm.h"

using namespace sample;

class STMTest : public QObject
{
    Q_OBJECT

public:
    STMTest();

private Q_SLOTS:

    void stupidGuidTest();

    void visitorTest();
    void bind1Test();
    void bind2Test();
    void bindRetryTest();
    void atomicallyTest();
    void coercingTest();
};

STMTest::STMTest()
{
}

template <typename Ret>
Ret withSuccess(const stm::RunResult<Ret>& result)
{
    if (result.retry)
        throw std::runtime_error("Result is invalid.");

    return result.result.value();
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

void STMTest::visitorTest()
{
    stm::STML<int>                 a1 = stm::pureF(10);
    stm::STML<stm::TVar<std::any>> a2 = stm::newTVar(10);

    stm::Context context1;
    auto ustamp1 = context1.newGUID();
    auto snapshot1 = context1.takeSnapshot();
    stm::AtomicRuntime runtime1(context1, ustamp1, snapshot1);

    int result1 = withSuccess(stm::runSTML<int, stm::StmlVisitor>(runtime1, a1));
    QVERIFY(result1 == 10);

    stm::Context context2;
    auto ustamp2 = context2.newGUID();
    auto snapshot2 = context2.takeSnapshot();
    stm::AtomicRuntime runtime2(context2, ustamp2, snapshot2);
    stm::TVar<std::any> result2 = withSuccess(stm::runSTML<stm::TVar<std::any>, stm::StmlVisitor>(runtime2, a2));
    QVERIFY(result2.id.size() == 32);
}

void STMTest::bind1Test()
{
    std::function<stm::STML<std::string>(int)> f = [](int)
    {
        return stm::pureF(std::string("abc"));
    };

    stm::STML<std::string> s = stm::bind(stm::pureF(10), f);

    stm::Context context;
    auto snapshot = context.takeSnapshot();
    stm::AtomicRuntime runtime(context, context.newGUID(), snapshot);

    auto result = withSuccess(stm::runSTML<std::string, stm::StmlVisitor>(runtime, s));
    QVERIFY(result == "abc");
}

void STMTest::bind2Test()
{
    std::function<stm::STML<std::any>(stm::TVar<std::any>)> f =
            [](const stm::TVar<std::any>& tvar)
    {
        return stm::readTVar(tvar);
    };

    auto x = stm::newTVar(10);

    stm::STML<std::any> s = stm::bind(x, f);

    stm::Context context;
    auto snapshot = context.takeSnapshot();
    stm::AtomicRuntime runtime(context, context.newGUID(), snapshot);

    std::any result = withSuccess(stm::runSTML<std::any, stm::StmlVisitor>(runtime, s));
    QVERIFY(std::any_cast<int>(result) == 10);
}

void STMTest::bindRetryTest()
{
    std::function<stm::STML<stm::TVar<std::any>>(stm::TVar<std::any>)> f =
            [](const stm::TVar<std::any>)
    {
        return stm::retry<stm::TVar<std::any>>();
    };

    auto x = stm::newTVar(10);
    auto s = stm::bind(x, f);

    stm::Context context;
    auto snapshot = context.takeSnapshot();
    stm::AtomicRuntime runtime(context, context.newGUID(), snapshot);

    bool success = false;
    try
    {
        withSuccess(stm::runSTML<stm::TVar<std::any>, stm::StmlVisitor>(runtime, s));
    }
    catch (...)
    {
        success = true;
    }
    QVERIFY(success);
}

void STMTest::atomicallyTest()
{
    std::function<stm::STML<std::any>(stm::TVar<std::any>)> f =
            [](const stm::TVar<std::any>& tvar)
    {
        return stm::readTVar(tvar);
    };

    auto x = stm::newTVar(10);

    stm::STML<std::any> s = stm::bind(x, f);

    stm::Context context;
    auto result = stm::atomically(context, s);
    QVERIFY(std::any_cast<int>(result) == 10);
}

void STMTest::coercingTest()
{
    using TVarInt = stm::TVar<int>;
    using STMLInt = stm::STML<int>;

    std::function<STMLInt(TVarInt)> f1 =
            [](const TVarInt& tvar)
    {
        return stm::readTVarT(tvar);
    };

    std::function<stm::STML<fp::Unit>(TVarInt)> f2 =
            [](const TVarInt& tvar)
    {
        return stm::writeTVarT(tvar, 20);
    };

    std::function<stm::STML<fp::Unit>(TVarInt)> f3 =
            [](const TVarInt&)
    {
        return stm::retry<fp::Unit>();
    };

    stm::STML<TVarInt>  m1 = stm::newTVarT(10);
    stm::STML<int>      m2 = stm::bind(m1, f1);
    stm::STML<fp::Unit> x1 = stm::bind(m1, f2);
    stm::STML<fp::Unit> y1 = stm::bind(m1, f3);

    stm::Context context;
    int  result1 = stm::atomically(context, m2);

    std::cout << "\nRunning.\n";
    stm::atomically(context, x1);

//  Will run forever because of retry.
//    stm::atomically(context, y1);

    QVERIFY(result1 == 10);
}

QTEST_APPLESS_MAIN(STMTest)

#include "tst_stm.moc"

