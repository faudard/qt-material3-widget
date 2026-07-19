#include <QtTest/QtTest>

#include "qtmaterial/tooling/qtmaterialcomponentregistry.h"

class tst_ComponentRegistry : public QObject {
    Q_OBJECT
private slots:
    void defaultRegistryContainsKnownComponents()
    {
        const auto registry = QtMaterial::defaultComponentRegistry();
        QVERIFY(!registry.isEmpty());
        QVERIFY(QtMaterial::findComponentDescriptor(registry, QStringLiteral("button.filled")) != nullptr);
        QVERIFY(QtMaterial::findComponentDescriptor(registry, QStringLiteral("navigation.rail")) != nullptr);
    }

    void maturityRoundTrip()
    {
        QCOMPARE(QtMaterial::componentMaturityToString(QtMaterial::ComponentMaturity::Usable), QStringLiteral("usable"));
        QCOMPARE(QtMaterial::componentMaturityFromString(QStringLiteral("partial")), QtMaterial::ComponentMaturity::Partial);
        QVERIFY(QtMaterial::isReleaseBlockingMaturity(QtMaterial::ComponentMaturity::Planned));
        QVERIFY(!QtMaterial::isReleaseBlockingMaturity(QtMaterial::ComponentMaturity::Usable));
    }
};

QTEST_MAIN(tst_ComponentRegistry)
#include "tst_componentregistry.moc"
