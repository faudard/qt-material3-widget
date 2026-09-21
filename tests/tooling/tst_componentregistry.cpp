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
        QVERIFY(QtMaterial::findComponentDescriptor(registry, QStringLiteral("data.carousel")) != nullptr);
        QVERIFY(QtMaterial::findComponentDescriptor(registry, QStringLiteral("surface.navigation-drawer")) != nullptr);

        const auto* filled = QtMaterial::findComponentDescriptor(registry, QStringLiteral("button.filled"));
        QVERIFY(filled != nullptr);
        QCOMPARE(filled->maturity, QtMaterial::ComponentMaturity::Complete);

        const auto* rail = QtMaterial::findComponentDescriptor(registry, QStringLiteral("navigation.rail"));
        QVERIFY(rail != nullptr);
        QCOMPARE(rail->maturity, QtMaterial::ComponentMaturity::Usable);

        const auto* snackbar = QtMaterial::findComponentDescriptor(registry, QStringLiteral("surface.snackbar"));
        QVERIFY(snackbar != nullptr);
        QCOMPARE(snackbar->maturity, QtMaterial::ComponentMaturity::Usable);

        const auto* chip = QtMaterial::findComponentDescriptor(registry, QStringLiteral("compact.chip"));
        QVERIFY(chip != nullptr);
        QCOMPARE(chip->maturity, QtMaterial::ComponentMaturity::Usable);
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
