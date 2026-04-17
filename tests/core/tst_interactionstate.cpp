#include <QtTest/QtTest>
#include "qtmaterial/core/qtmaterialinteractionstate.h"
class tst_InteractionState : public QObject
{
    Q_OBJECT
private slots:
    void defaultsAreSane();
    void updatesFlags();
};
void tst_InteractionState::defaultsAreSane()
{
    QtMaterial::QtMaterialInteractionState state;
    QVERIFY(!state.isHovered());
    QVERIFY(!state.isPressed());
    QVERIFY(state.isEnabled());
}
void tst_InteractionState::updatesFlags()
{
    QtMaterial::QtMaterialInteractionState state;
    state.setHovered(true);
    state.setPressed(true);
    state.setChecked(true);
    QVERIFY(state.isHovered());
    QVERIFY(state.isPressed());
    QVERIFY(state.isChecked());
}
QTEST_MAIN(tst_InteractionState)
#include "tst_interactionstate.moc"
