#include <QtTest/QtTest>
#include <QLineEdit>

#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"

class tst_OutlinedTextField : public QObject
{
    Q_OBJECT
private slots:
    void basicConstruction();
    void accessibilityState();
    void forwardsFocusToLineEdit();
    void sizeHintRespectsShellHeight();
    void contentChangeInvalidatesShellText();
};

void tst_OutlinedTextField::basicConstruction()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
    widget.setLabelText(QStringLiteral("Label"));
    QVERIFY(widget.lineEdit() != nullptr);
    QCOMPARE(widget.focusProxy(), widget.lineEdit());
}

void tst_OutlinedTextField::accessibilityState()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
    widget.setLabelText(QStringLiteral("Email"));
    widget.setSupportingText(QStringLiteral("We will not spam you"));
    QCOMPARE(widget.accessibleName(), QStringLiteral("Email"));
    QCOMPARE(widget.lineEdit()->accessibleName(), QStringLiteral("Email"));
    QCOMPARE(widget.lineEdit()->accessibleDescription(), QStringLiteral("We will not spam you"));

    widget.setErrorText(QStringLiteral("Invalid email"));
    widget.setHasErrorState(true);
    QCOMPARE(widget.lineEdit()->accessibleDescription(), QStringLiteral("Invalid email"));
}

void tst_OutlinedTextField::forwardsFocusToLineEdit()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
    widget.setLabelText(QStringLiteral("Email"));
    widget.resize(widget.sizeHint());
    widget.show();
    QVERIFY(QTest::qWaitForWindowActive(&widget));

    widget.setFocus(Qt::TabFocusReason);
    QVERIFY(QTest::qWaitFor([&widget]() {
        return widget.lineEdit() && widget.lineEdit()->hasFocus();
    }));
}

void tst_OutlinedTextField::sizeHintRespectsShellHeight()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
    widget.setLabelText(QStringLiteral("Email"));
    const QSize hint = widget.sizeHint();
    QVERIFY(hint.height() >= 80);
    QVERIFY(hint.width() >= 120);
}

void tst_OutlinedTextField::contentChangeInvalidatesShellText()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
    widget.setLabelText(QStringLiteral("Email"));
    widget.resize(widget.sizeHint());
    const QSize first = widget.sizeHint();

    widget.setLabelText(QStringLiteral("Very long email address label that should change width"));
    const QSize second = widget.sizeHint();

    QVERIFY(second.width() >= first.width());
}

QTEST_MAIN(tst_OutlinedTextField)
#include "tst_outlinedtextfield.moc"
