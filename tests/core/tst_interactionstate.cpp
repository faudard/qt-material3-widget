#include <QtTest>
#include "qtmaterial/core/qtmaterialinteractionstate.h"

class tst_InteractionState : public QObject
{
    Q_OBJECT

private slots:
    void defaultsAreSane();
    void checkedRequiresCheckable();
    void disablingClearsTransientFlags();
    void disabledStateRejectsTransientFlags();
};

void tst_InteractionState::defaultsAreSane()
{
    QtMaterial::QtMaterialInteractionState state;

    QVERIFY(!state.isHovered());
    QVERIFY(!state.isFocused());
    QVERIFY(!state.isPressed());
    QVERIFY(!state.isChecked());
    QVERIFY(!state.isCheckable());
    QVERIFY(state.isEnabled());
    QVERIFY(!state.isSelected());
    QVERIFY(!state.hasError());
}

void tst_InteractionState::checkedRequiresCheckable()
{
    QtMaterial::QtMaterialInteractionState state;

    state.setChecked(true);
    QVERIFY(!state.isChecked());

    state.setCheckable(true);
    QVERIFY(state.isCheckable());
    QVERIFY(!state.isChecked());

    state.setChecked(true);
    QVERIFY(state.isChecked());

    state.setCheckable(false);
    QVERIFY(!state.isCheckable());
    QVERIFY(!state.isChecked());
}

void tst_InteractionState::disablingClearsTransientFlags()
{
    QtMaterial::QtMaterialInteractionState state;

    state.setCheckable(true);
    state.setChecked(true);
    state.setHovered(true);
    state.setFocused(true);
    state.setPressed(true);

    QVERIFY(state.isHovered());
    QVERIFY(state.isFocused());
    QVERIFY(state.isPressed());
    QVERIFY(state.isChecked());

    state.setEnabled(false);

    QVERIFY(!state.isEnabled());
    QVERIFY(!state.isHovered());
    QVERIFY(!state.isFocused());
    QVERIFY(!state.isPressed());
    QVERIFY(state.isChecked()); // checked survives disable
}

void tst_InteractionState::disabledStateRejectsTransientFlags()
{
    QtMaterial::QtMaterialInteractionState state;
    state.setEnabled(false);

    state.setHovered(true);
    state.setFocused(true);
    state.setPressed(true);

    QVERIFY(!state.isHovered());
    QVERIFY(!state.isFocused());
    QVERIFY(!state.isPressed());
}

QTEST_MAIN(tst_InteractionState)
#include "tst_interactionstate.moc"