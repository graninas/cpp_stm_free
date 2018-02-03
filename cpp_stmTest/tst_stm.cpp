#include <QString>
#include <QtTest>

#include "common.h"
#include "address.h"
#include "person.h"
#include "account.h"

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

void STMTest::stmTest()
{
    QVERIFY(20 == 20);
}

QTEST_APPLESS_MAIN(STMTest)

#include "tst_stm.moc"

