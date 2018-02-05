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

const std::function<stm::STM<Unit>(stm::TVar<Fork>) =
        [](const stm::TVar<Fork>& tFork)
{
    auto fork = stm::readTVar(tFork);
    switch (fork.state)
    {
        case ForkState::Taken:
            stm::writeTVar(tFork, Fork(fork.name, ForkState::Free));
        break;
        case ForkState::Free:
        break;
    }
    return unit();
};

void STMTest::stmTest()
{
    auto fork1 = stm::newTVarIO<Fork>();
    stm::STM scenario = takeFork(fork1);
    auto state = stm::atomically(scenario);

    QVERIFY(state == ForkState::Taken);
}

QTEST_APPLESS_MAIN(STMTest)

#include "tst_stm.moc"

