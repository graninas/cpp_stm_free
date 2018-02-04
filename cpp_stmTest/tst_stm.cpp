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

ForkState takeFork(stm::TVar<Fork>& const fork)
{
    return ForkState::Free;
}

void STMTest::stmTest()
{
    auto fork1 = newTVarIO();
    stm::STM scenario = takeFork(fork1);
    auto state = stm::atomically(scenario);

    QVERIFY(state == ForkState::Taken);
}

QTEST_APPLESS_MAIN(STMTest)

#include "tst_stm.moc"

