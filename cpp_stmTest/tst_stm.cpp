#include <QString>
#include <QtTest>

#include <variant>
#include <iostream>

#include "common.h"
#include "stm/stm.h"

class STMTest : public QObject
{
    Q_OBJECT

public:
    STMTest();

private Q_SLOTS:

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

    Context ctx;
    auto result = atomically(ctx, m3);
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

    Context ctx;
    auto result = atomically(ctx, mResult);
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

    Context ctx;
    auto result = atomically(ctx, mResult);
    QVERIFY(result == std::string("10abc"));
}

void STMTest::bothVoidedCombinatorTest()
{
    using namespace stm;

    auto mResult = bothVoided<int, std::string>
            (pure(10),
             pure(std::string("abc")));

    Context ctx;
    Unit result = atomically(ctx, mResult);
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

    Context ctx;
    auto result = atomically(ctx, mResult);
    QVERIFY(result == 15);
}

QTEST_APPLESS_MAIN(STMTest)

#include "tst_stm.moc"

