#include <QtTest/QtTest>

#include <QObject>

#include "qtmaterial/core/qtmaterialautomation.h"
#include "qtmaterial/foundation/qtmaterialinteractionstate.h"

class tst_InteractionStateContracts : public QObject
{
    Q_OBJECT

private slots:
    void disabledClearsTransientState();
    void checkableGuardsCheckedAndIndeterminate();
    void automationPropertyUsesStableVocabulary();
};

void tst_InteractionStateContracts::disabledClearsTransientState()
{
    QtMaterial::QtMaterialInteractionState state;
    state.setHovered(true);
    state.setFocused(true);
    state.setPressed(true);
    state.setDragged(true);
    state.setSelected(true);

    state.setEnabled(false);

    QVERIFY(!state.isEnabled());
    QVERIFY(!state.isHovered());
    QVERIFY(!state.isFocused());
    QVERIFY(!state.isPressed());
    QVERIFY(!state.isDragged());
    QVERIFY(!state.isSelected());
}

void tst_InteractionStateContracts::checkableGuardsCheckedAndIndeterminate()
{
    QtMaterial::QtMaterialInteractionState state;

    state.setChecked(true);
    state.setIndeterminate(true);
    QVERIFY(!state.isChecked());
    QVERIFY(!state.isIndeterminate());

    state.setCheckable(true);
    state.setChecked(true);
    state.setIndeterminate(true);
    QVERIFY(state.isChecked());
    QVERIFY(state.isIndeterminate());

    state.setCheckable(false);
    QVERIFY(!state.isChecked());
    QVERIFY(!state.isIndeterminate());
}

void tst_InteractionStateContracts::automationPropertyUsesStableVocabulary()
{
    QObject object;
    QtMaterial::QtMaterialInteractionState state;

    state.setCheckable(true);
    state.setChecked(true);
    state.setError(true);
    state.setReadOnly(true);

    QtMaterial::QtMaterialAutomation::syncState(&object, state);

    QCOMPARE(
        QtMaterial::QtMaterialAutomation::state(&object),
        QStringLiteral("enabled checkable checked error readOnly"));
}

QTEST_MAIN(tst_InteractionStateContracts)
#include "tst_interactionstate_contracts.moc"
