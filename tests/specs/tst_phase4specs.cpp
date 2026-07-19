#include <QtTest>

#include "qtmaterial/specs/qtmaterialbuttonspecresolver.h"

class TestSpecResolver : public QObject
{
    Q_OBJECT

private slots:
    void constructs();
};

void TestSpecResolver::constructs()
{
    const QtMaterial::ButtonSpecResolver resolver;
    Q_UNUSED(resolver);
    QVERIFY(true);
}

QTEST_MAIN(TestSpecResolver)
#include "tst_phase4specs.moc"
