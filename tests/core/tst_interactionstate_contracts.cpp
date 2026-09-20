#include <QtTest/QtTest>

#include <QObject>

#include "qtmaterial/testing/qtmaterialautomation.h"
#include "qtmaterial/foundation/qtmaterialinteractionstate.h"

class tst_InteractionStateContracts : public QObject
{
    Q_OBJECT

private slots:
    void disabledClearsTransientStateButKeepsPersistentSelection();
    void checkableGuardsCheckedAndIndeterminate();
    void readOnlySuppressesPressAndDrag();
    void persistentStatesSurviveDisabled();
    void automationPropertyUsesStableVocabulary();
};

void tst_InteractionStateContracts::disabledClearsTransientStateButKeepsPersistentSelection()
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
    QVERIFY(state.isSelected());
    QVERIFY(!state.hasTransientState());
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

void tst_InteractionStateContracts::readOnlySuppressesPressAndDrag()
{
    QtMaterial::QtMaterialInteractionState state;
    state.setPressed(true);
    state.setDragged(true);
    QVERIFY(state.isPressed());
    QVERIFY(state.isDragged());

    state.setReadOnly(true);
    QVERIFY(state.isReadOnly());
    QVERIFY(!state.isPressed());
    QVERIFY(!state.isDragged());
    QVERIFY(!state.isInteractive());

    state.setPressed(true);
    state.setDragged(true);
    QVERIFY(!state.isPressed());
    QVERIFY(!state.isDragged());
}

void tst_InteractionStateContracts::persistentStatesSurviveDisabled()
{
    QtMaterial::QtMaterialInteractionState state;
    state.setCheckable(true);
    state.setChecked(true);
    state.setSelected(true);
    state.setError(true);
    state.setBusy(true);
    state.setExpanded(true);
    state.setInvalid(true);

    state.setEnabled(false);

    QVERIFY(!state.isEnabled());
    QVERIFY(state.isChecked());
    QVERIFY(state.isSelected());
    QVERIFY(state.hasError());
    QVERIFY(state.isBusy());
    QVERIFY(state.isExpanded());
    QVERIFY(state.isInvalid());
}

void tst_InteractionStateContracts::automationPropertyUsesStableVocabulary()
{
    QObject object;
    QtMaterial::QtMaterialInteractionState state;

    state.setCheckable(true);
    state.setChecked(true);
    state.setSelected(true);
    state.setError(true);
    state.setReadOnly(true);
    state.setIndeterminate(true);
    state.setBusy(true);
    state.setExpanded(true);
    state.setInvalid(true);

    QtMaterial::QtMaterialAutomation::syncState(&object, state);

    QCOMPARE(
        QtMaterial::QtMaterialAutomation::state(&object),
        QStringLiteral(
            "enabled checkable checked selected error readOnly "
            "indeterminate busy expanded invalid"));
}

QTEST_MAIN(tst_InteractionStateContracts)
#include "tst_interactionstate_contracts.moc"
