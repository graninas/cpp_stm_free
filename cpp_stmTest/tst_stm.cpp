#include <QString>
#include <QtTest>

#include <variant>
#include <iostream>

#include "common.h"
#include "stm/impl/stupid_guid.h"
#include "stm/free/stm.h"
#include "stm/church/stm.h"

//#include "stm_free_church.h"
//#include "stm_church.h"
//#include "stm_bind.h"
//#include "stm_interpreter.h"
//#include "stm.h"

class STMTest : public QObject
{
    Q_OBJECT

public:
    STMTest();

private Q_SLOTS:

    void stupidGuidTest();

    void atomicallyTest();
    void coercingTest();

//    void sequenceCombinatorTest();
//    void whenCombinatorTest();
//    void unlessCombinatorTest();
//    void bothCombinatorTest();
//    void bothTVarsCombinatorTest();
//    void bothVoidedCombinatorTest();
//    void modifyTVarTest();

//    void churchTest();
};

STMTest::STMTest()
{
}

using TVarInt = stm::TVar<int>;
using TVarStr = stm::TVar<std::string>;

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

void STMTest::atomicallyTest()
{
    std::function<stm::free::STML<int>(TVarInt)> f =
            [](const TVarInt& tvar)
    {
        return stm::free::readTVar(tvar);
    };

    auto x = stm::free::newTVar(10);

    stm::free::STML<int> s = stm::free::bind(x, f);

    stm::Context context;
    auto result = stm::free::atomically(context, s);
    QVERIFY(result == 10);
}

void STMTest::coercingTest()
{
    stm::free::STML<TVarInt>   m1 = stm::free::newTVar(10);
    stm::free::STML<int>       m2 = stm::free::bind<TVarInt, int>(m1, stm::free::mReadTVar);
    stm::free::STML<stm::Unit> x1 = stm::free::bind<TVarInt, stm::Unit>(m1, stm::free::mWriteTVarV(20));
    stm::free::STML<stm::Unit> y1 = stm::free::sequence<TVarInt, stm::Unit>(m1, stm::free::mRetry);

    stm::Context context;
    int result1 = stm::free::atomically(context, m2);

    stm::free::atomically(context, x1);

//  Will run forever because of retry.
//    stm::atomically(context, y1);

    QVERIFY(result1 == 10);
}

//void STMTest::sequenceCombinatorTest()
//{
//    using namespace stm;

//    auto m1 = newTVar(10);
//    auto m2 = stm::bind<TVarInt, TVarInt>(m1, [](const auto& tvar)
//    {
//        auto mm1 = writeTVar(tvar, 20);
//        return sequence(mm1, pure(tvar));
//    });
//    auto m3 = stm::bind<TVarInt, int>(m2, mReadTVar);

//    int  result = run(m3);
//    QVERIFY(result == 20);
//}

//void STMTest::whenCombinatorTest()
//{
//    using namespace stm;

//    auto m1 = when(pure(true),  newTVar(10));
//    auto m2 = when(pure(false), newTVar(10));

//    Context context1;
//    Context context2;

//    TVars tvars1_1 = context1.takeSnapshot();
//    TVars tvars2_1 = context2.takeSnapshot();

//    QVERIFY(tvars1_1.size() == 0);
//    QVERIFY(tvars2_1.size() == 0);

//    atomically(context1, m1);
//    atomically(context2, m2);

//    TVars tvars1_2 = context1.takeSnapshot();
//    TVars tvars2_2 = context2.takeSnapshot();

//    QVERIFY(tvars1_2.size() == 1);
//    QVERIFY(tvars2_2.size() == 0);
//}

//void STMTest::unlessCombinatorTest()
//{
//    using namespace stm;

//    auto m1 = unless(pure(true),  newTVar(10));
//    auto m2 = unless(pure(false), newTVar(10));

//    Context context1;
//    Context context2;

//    TVars tvars1_1 = context1.takeSnapshot();
//    TVars tvars2_1 = context2.takeSnapshot();

//    QVERIFY(tvars1_1.size() == 0);
//    QVERIFY(tvars2_1.size() == 0);

//    atomically(context1, m1);
//    atomically(context2, m2);

//    TVars tvars1_2 = context1.takeSnapshot();
//    TVars tvars2_2 = context2.takeSnapshot();

//    QVERIFY(tvars1_2.size() == 0);
//    QVERIFY(tvars2_2.size() == 1);
//}

//void STMTest::bothCombinatorTest()
//{
//    using namespace stm;

//    auto mResult = both<int, std::string, std::string>
//            (pure(10),
//             pure(std::string("abc")),
//             [](int i, const std::string& s)
//    {
//        return std::to_string(i) + s;
//    });

//    auto result = run(mResult);
//    QVERIFY(result == std::string("10abc"));
//}

//void STMTest::bothTVarsCombinatorTest()
//{
//    using namespace stm;

//    auto mResult = withTVars<int, std::string, std::string>(
//                newTVar(10),
//                newTVar(std::string("abc")),
//                [](int i, const std::string& s)
//                {
//                    return std::to_string(i) + s;
//                });

//    auto result = run(mResult);
//    QVERIFY(result == std::string("10abc"));
//}

//void STMTest::bothVoidedCombinatorTest()
//{
//    using namespace stm;

//    auto mResult = bothVoided<int, std::string>
//            (pure(10),
//             pure(std::string("abc")));

//    Unit result = run(mResult);
//    Q_UNUSED(result);
//}

//void STMTest::modifyTVarTest()
//{
//    using namespace stm;

//    std::function<int(int)> f = [](int i) { return i + 5; };

//    auto m1 = newTVar(10);
//    auto mResult = bind<TVar<int>, int>(m1, [=](auto tvar)
//    {
//        auto m3 = modifyTVar(tvar, f);
//        return bind<Unit, int>(m3, [=](const auto&)
//        {
//            return readTVar(tvar);
//        });
//    });

//    auto result = run(mResult);
//    QVERIFY(result == 15);
//}

//void STMTest::churchTest()
//{
//    stm::Context ctx;

//    auto trans1 = stm::church::newTVar<int>(10);
//    stm::TVar<int> tvar = stm::church::atomically(ctx, trans1);

//    auto trans2 = stm::church::readTVar(tvar);
//    int result1 = stm::church::atomically(ctx, trans2);
//    QVERIFY(result1 == 10);

//    auto trans3 = stm::church::writeTVar(tvar, 20);
//    stm::church::atomically(ctx, trans3);

//    int result2 = stm::church::atomically(ctx, trans2);
//    QVERIFY(result2 == 20);
//}

QTEST_APPLESS_MAIN(STMTest)

#include "tst_stm.moc"

