#include <QtTest>

#include "qtmaterial/core/qtmaterialcontrol.h"
#include "qtmaterial/core/qtmaterialwidget.h"
#include "qtmaterial/foundation/qtmaterialmetadataproperties.h"

using namespace QtMaterial;

class tst_MetadataRuntime : public QObject
{
    Q_OBJECT

private slots:
    void widgetPublishesMetadataWithoutAutomationDependency();
    void controlPublishesInteractionState();
};

void tst_MetadataRuntime::widgetPublishesMetadataWithoutAutomationDependency()
{
    QtMaterialWidget widget;
    widget.setMaterialComponent(QStringLiteral("Widget"));
    widget.setMaterialVariant(QStringLiteral("Test"));
    widget.setMaterialRole(QStringLiteral("content"));
    widget.setMaterialTestId(QStringLiteral("widget-1"));

    QCOMPARE(
        widget.property(QtMaterialMetadataPropertyNames::component()).toString(),
        QStringLiteral("Widget"));
    QCOMPARE(
        widget.property(QtMaterialMetadataPropertyNames::variant()).toString(),
        QStringLiteral("Test"));
    QCOMPARE(
        widget.property(QtMaterialMetadataPropertyNames::role()).toString(),
        QStringLiteral("content"));
    QCOMPARE(
        widget.property(QtMaterialMetadataPropertyNames::testId()).toString(),
        QStringLiteral("widget-1"));
}

void tst_MetadataRuntime::controlPublishesInteractionState()
{
    QtMaterialControl control;
    control.setEnabled(false);

    const QString state =
        control.property(QtMaterialMetadataPropertyNames::state()).toString();
    QVERIFY(!state.contains(QStringLiteral("enabled")));
}

QTEST_MAIN(tst_MetadataRuntime)
#include "tst_metadataruntime.moc"
