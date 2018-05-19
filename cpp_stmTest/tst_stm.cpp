#include <QString>
#include <QtTest>

#include <variant>
#include <iostream>

#include "common.h"
#include "stupid_guid.h"
#include "stm_free.h"
#include "stm_bind.h"
#include "stm_interpreter.h"
#include "stm.h"

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

    void sequenceCombinatorTest();
    void whenCombinatorTest();
    void unlessCombinatorTest();
    void bothCombinatorTest();
    void bothTVarsCombinatorTest();
    void bothVoidedCombinatorTest();
    void modifyTVarTest();
};

STMTest::STMTest()
{
}

using TVarInt = stm::TVar<int>;
using TVarStr = stm::TVar<std::string>;

template <typename A>
A run(const stm::STML<A>& stml)
{
    stm::Context context;
    return stm::atomically(context, stml);
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
    stm::STML<int>     m1 = stm::pure(10);
    stm::STML<TVarInt> m2 = stm::newTVar(10);

    stm::Context context1;
    auto ustamp1 = context1.newGUID();
    auto snapshot1 = context1.takeSnapshot();
    stm::AtomicRuntime runtime1(context1, ustamp1, snapshot1);

    int result1 = withSuccess(stm::free::runSTML<int, stm::free::StmlVisitor>(runtime1, m1));
    QVERIFY(result1 == 10);

    stm::Context context2;
    auto ustamp2 = context2.newGUID();
    auto snapshot2 = context2.takeSnapshot();
    stm::AtomicRuntime runtime2(context2, ustamp2, snapshot2);
    TVarInt result2 =
            withSuccess(stm::free::runSTML<TVarInt, stm::free::StmlVisitor>(runtime2, m2));
    QVERIFY(result2.id.size() == 32);
}

void STMTest::bind1Test()
{
    std::function<stm::STML<std::string>(int)> f = [](int)
    {
        return stm::pure(std::string("abc"));
    };

    stm::STML<std::string> s = stm::bind(stm::pure(10), f);

    stm::Context context;
    auto snapshot = context.takeSnapshot();
    stm::AtomicRuntime runtime(context, context.newGUID(), snapshot);

    auto result = withSuccess(stm::free::runSTML<std::string, stm::free::StmlVisitor>(runtime, s));
    QVERIFY(result == "abc");
}

void STMTest::bind2Test()
{
    std::function<stm::STML<int>(TVarInt)> f =
            [](const TVarInt& tvar)
    {
        return stm::readTVar(tvar);
    };

    auto x = stm::newTVar(10);

    stm::STML<int> s = stm::bind(x, f);

    stm::Context context;
    auto snapshot = context.takeSnapshot();
    stm::AtomicRuntime runtime(context, context.newGUID(), snapshot);

    int result = withSuccess(stm::free::runSTML<int, stm::free::StmlVisitor>(runtime, s));
    QVERIFY(result == 10);
}

void STMTest::bindRetryTest()
{
    std::function<stm::STML<TVarInt>(TVarInt)> f =
            [](const TVarInt)
    {
        return stm::retry<TVarInt>();
    };

    auto x = stm::newTVar(10);
    auto s = stm::bind(x, f);

    stm::Context context;
    auto snapshot = context.takeSnapshot();
    stm::AtomicRuntime runtime(context, context.newGUID(), snapshot);

    bool success = false;
    try
    {
        withSuccess(stm::free::runSTML<TVarInt, stm::free::StmlVisitor>(runtime, s));
    }
    catch (...)
    {
        success = true;
    }
    QVERIFY(success);
}

void STMTest::atomicallyTest()
{
    std::function<stm::STML<int>(TVarInt)> f =
            [](const TVarInt& tvar)
    {
        return stm::readTVar(tvar);
    };

    auto x = stm::newTVar(10);

    stm::STML<int> s = stm::bind(x, f);

    stm::Context context;
    auto result = stm::atomically(context, s);
    QVERIFY(result == 10);
}

void STMTest::coercingTest()
{
    stm::STML<TVarInt>   m1 = stm::newTVar(10);
    stm::STML<int>       m2 = stm::bind<TVarInt, int>(m1, stm::mReadTVar);
    stm::STML<stm::Unit> x1 = stm::bind<TVarInt, stm::Unit>(m1, stm::mWriteTVarV(20));
    stm::STML<stm::Unit> y1 = stm::sequence<TVarInt, stm::Unit>(m1, stm::mRetry);

    stm::Context context;
    int result1 = stm::atomically(context, m2);

    stm::atomically(context, x1);

//  Will run forever because of retry.
//    stm::atomically(context, y1);

    QVERIFY(result1 == 10);
}

void STMTest::sequenceCombinatorTest()
{
    using namespace stm;

    auto m1 = newTVar(10);
    auto m2 = stm::bind<TVarInt, TVarInt>(m1, [](const auto& tvar)
    {
        auto mm1 = writeTVar(tvar, 20);
        return sequence(mm1, pure(tvar));
    });
    auto m3 = stm::bind<TVarInt, int>(m2, mReadTVar);

    int  result = run(m3);
    QVERIFY(result == 20);
}

void STMTest::whenCombinatorTest()
{
    using namespace stm;

    auto m1 = when(pure(true),  newTVar(10));
    auto m2 = when(pure(false), newTVar(10));

    Context context1;
    Context context2;

    TVars tvars1_1 = context1.takeSnapshot();
    TVars tvars2_1 = context2.takeSnapshot();

    QVERIFY(tvars1_1.size() == 0);
    QVERIFY(tvars2_1.size() == 0);

    atomically(context1, m1);
    atomically(context2, m2);

    TVars tvars1_2 = context1.takeSnapshot();
    TVars tvars2_2 = context2.takeSnapshot();

    QVERIFY(tvars1_2.size() == 1);
    QVERIFY(tvars2_2.size() == 0);
}

void STMTest::unlessCombinatorTest()
{
    using namespace stm;

    auto m1 = unless(pure(true),  newTVar(10));
    auto m2 = unless(pure(false), newTVar(10));

    Context context1;
    Context context2;

    TVars tvars1_1 = context1.takeSnapshot();
    TVars tvars2_1 = context2.takeSnapshot();

    QVERIFY(tvars1_1.size() == 0);
    QVERIFY(tvars2_1.size() == 0);

    atomically(context1, m1);
    atomically(context2, m2);

    TVars tvars1_2 = context1.takeSnapshot();
    TVars tvars2_2 = context2.takeSnapshot();

    QVERIFY(tvars1_2.size() == 0);
    QVERIFY(tvars2_2.size() == 1);
}

void STMTest::bothCombinatorTest()
{
    using namespace stm;

    auto mResult = both<int, std::string, std::string>
            (pure(10),
             pure(std::string("abc")),
             [](int i, const std::string& s)
    {
        return std::to_string(i) + s;
    });

    auto result = run(mResult);
    QVERIFY(result == std::string("10abc"));
}

void STMTest::bothTVarsCombinatorTest()
{
    using namespace stm;

    auto mResult = withTVars<int, std::string, std::string>(
                newTVar(10),
                newTVar(std::string("abc")),
                [](int i, const std::string& s)
                {
                    return std::to_string(i) + s;
                });

    auto result = run(mResult);
    QVERIFY(result == std::string("10abc"));
}

void STMTest::bothVoidedCombinatorTest()
{
    using namespace stm;

    auto mResult = bothVoided<int, std::string>
            (pure(10),
             pure(std::string("abc")));

    Unit result = run(mResult);
    Q_UNUSED(result);
}

void STMTest::modifyTVarTest()
{
    using namespace stm;

    std::function<int(int)> f = [](int i) { return i + 5; };

    auto m1 = newTVar(10);
    auto mResult = bind<TVar<int>, int>(m1, [=](auto tvar)
    {
        auto m3 = modifyTVar(tvar, f);
        return bind<Unit, int>(m3, [=](const auto&)
        {
            return readTVar(tvar);
        });
    });

    auto result = run(mResult);
    QVERIFY(result == 15);
}

QTEST_APPLESS_MAIN(STMTest)

#include "tst_stm.moc"

