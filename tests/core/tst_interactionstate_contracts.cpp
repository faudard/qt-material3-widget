#include <QtTest/QtTest>

#include "qtmaterial/core/qtmaterialinteractionstate.h"

class tst_InteractionStateContracts : public QObject
{
    Q_OBJECT

private slots:
    void disabledClearsTransientState();
    void checkableGuardsCheckedAndIndeterminate();
    void propertyStringUsesStableVocabulary();
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

void tst_InteractionStateContracts::propertyStringUsesStableVocabulary()
{
    QtMaterial::QtMaterialInteractionState state;
    state.setCheckable(true);
    state.setChecked(true);
    state.setError(true);
    state.setReadOnly(true);

    const QString value = state.toPropertyString();
    QVERIFY(value.contains(QStringLiteral("enabled")));
    QVERIFY(value.contains(QStringLiteral("checkable")));
    QVERIFY(value.contains(QStringLiteral("checked")));
    QVERIFY(value.contains(QStringLiteral("error")));
    QVERIFY(value.contains(QStringLiteral("readOnly")));
}

QTEST_MAIN(tst_InteractionStateContracts)
#include "tst_interactionstate_contracts.moc"
