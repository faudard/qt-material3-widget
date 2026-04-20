#include <QtTest/QtTest>
#include <QLineEdit>

#include "qtmaterial/widgets/inputs/qtmaterialfilledtextfield.h"

class tst_FilledTextField : public QObject
{
    Q_OBJECT
private slots:
    void basicConstruction();
    void accessibilityState();
    void forwardsFocusToLineEdit();
    void sizeHintRespectsShellHeight();
    void contentChangeInvalidatesShellText();
};

void tst_FilledTextField::basicConstruction()
{
    QtMaterial::QtMaterialFilledTextField widget(QStringLiteral("Name"));
    QVERIFY(widget.lineEdit() != nullptr);
    QCOMPARE(widget.focusProxy(), widget.lineEdit());
    QCOMPARE(widget.labelText(), QStringLiteral("Name"));
}

void tst_FilledTextField::accessibilityState()
{
    QtMaterial::QtMaterialFilledTextField widget;
    widget.setLabelText(QStringLiteral("Name"));
    widget.setSupportingText(QStringLiteral("Shown on your profile"));
    QCOMPARE(widget.accessibleName(), QStringLiteral("Name"));
    QCOMPARE(widget.lineEdit()->accessibleName(), QStringLiteral("Name"));
    QCOMPARE(widget.lineEdit()->accessibleDescription(), QStringLiteral("Shown on your profile"));

    widget.setErrorText(QStringLiteral("Required"));
    widget.setHasErrorState(true);
    QCOMPARE(widget.lineEdit()->accessibleDescription(), QStringLiteral("Required"));
}

void tst_FilledTextField::forwardsFocusToLineEdit()
{
    QtMaterial::QtMaterialFilledTextField widget;
    widget.setLabelText(QStringLiteral("Name"));
    widget.resize(widget.sizeHint());
    widget.show();
    QVERIFY(QTest::qWaitForWindowActive(&widget));

    widget.setFocus(Qt::TabFocusReason);
    QVERIFY(QTest::qWaitFor([&widget]() {
        return widget.lineEdit() && widget.lineEdit()->hasFocus();
    }));
}

void tst_FilledTextField::sizeHintRespectsShellHeight()
{
    QtMaterial::QtMaterialFilledTextField widget;
    widget.setLabelText(QStringLiteral("Name"));
    const QSize hint = widget.sizeHint();
    QVERIFY(hint.height() >= 80);
    QVERIFY(hint.width() >= 120);
}

void tst_FilledTextField::contentChangeInvalidatesShellText()
{
    QtMaterial::QtMaterialFilledTextField widget;
    widget.setLabelText(QStringLiteral("Name"));
    widget.resize(widget.sizeHint());
    const QSize first = widget.sizeHint();

    widget.setLabelText(QStringLiteral("Very long account display name label that should change width"));
    const QSize second = widget.sizeHint();

    QVERIFY(second.width() >= first.width());
}

QTEST_MAIN(tst_FilledTextField)
#include "tst_filledtextfield.moc"
