#include <QtTest/QtTest>
#include <QObject>

#include "qtmaterial/core/qtmaterialautomation.h"

class tst_AutomationMetadata : public QObject
{
    Q_OBJECT

private slots:
    void writesStableDynamicProperties();
    void syncsInteractionState();
};

void tst_AutomationMetadata::writesStableDynamicProperties()
{
    QObject object;
    QtMaterial::QtMaterialAutomation::setComponent(&object, QStringLiteral("Button"));
    QtMaterial::QtMaterialAutomation::setVariant(&object, QStringLiteral("Filled"));
    QtMaterial::QtMaterialAutomation::setRole(&object, QStringLiteral("primaryAction"));
    QtMaterial::QtMaterialAutomation::setTestId(&object, QStringLiteral("save-button"));

    QCOMPARE(QtMaterial::QtMaterialAutomation::component(&object), QStringLiteral("Button"));
    QCOMPARE(QtMaterial::QtMaterialAutomation::variant(&object), QStringLiteral("Filled"));
    QCOMPARE(QtMaterial::QtMaterialAutomation::role(&object), QStringLiteral("primaryAction"));
    QCOMPARE(QtMaterial::QtMaterialAutomation::testId(&object), QStringLiteral("save-button"));
}

void tst_AutomationMetadata::syncsInteractionState()
{
    QObject object;
    QtMaterial::QtMaterialInteractionState state;
    state.setHovered(true);
    state.setFocused(true);
    state.setCheckable(true);
    state.setChecked(true);

    QtMaterial::QtMaterialAutomation::syncState(&object, state);

    const QStringList names = QtMaterial::QtMaterialAutomation::stateNames(&object);
    QVERIFY(names.contains(QStringLiteral("enabled")));
    QVERIFY(names.contains(QStringLiteral("hovered")));
    QVERIFY(names.contains(QStringLiteral("focused")));
    QVERIFY(names.contains(QStringLiteral("checked")));
}

QTEST_MAIN(tst_AutomationMetadata)
#include "tst_automationmetadata.moc"
