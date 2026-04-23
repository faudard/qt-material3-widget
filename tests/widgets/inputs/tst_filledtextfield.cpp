#include <QtTest/QtTest>

#include <QLabel>
#include <QLineEdit>
#include <QToolButton>

#include "qtmaterial/widgets/inputs/qtmaterialfilledtextfield.h"

namespace {

template <typename T>
T* requireChild(QWidget& parent, const QString& name)
{
    return parent.findChild<T*>(name);
}

void realize(QWidget& widget)
{
    widget.resize(qMax(widget.width(), 360), qMax(widget.height(), widget.sizeHint().height()));
    widget.show();
    QTest::qWait(1);
    QCoreApplication::processEvents();
}

} // namespace

class tst_FilledTextField : public QObject
{
    Q_OBJECT

private slots:
    void basicConstruction();
    void accessibilityState();
    void forwardsFocusToLineEdit();
    void sizeHintRespectsShellHeight();
    void contentChangeInvalidatesShellText();

    void inheritsPrefixSuffixBehavior();
    void inheritsPasswordEndActionBehavior();
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
    QCOMPARE(widget.lineEdit()->accessibleDescription(),
             QStringLiteral("Shown on your profile"));

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

void tst_FilledTextField::inheritsPrefixSuffixBehavior()
{
    QtMaterial::QtMaterialFilledTextField widget;
    widget.setLabelText(QStringLiteral("Price"));
    realize(widget);

    const int baseEditorWidth = widget.lineEdit()->geometry().width();

    widget.setPrefixText(QStringLiteral("$"));
    widget.setSuffixText(QStringLiteral("USD"));
    QCoreApplication::processEvents();

    QLabel* prefixLabel = requireChild<QLabel>(widget, QStringLiteral("prefixLabel"));
    QLabel* suffixLabel = requireChild<QLabel>(widget, QStringLiteral("suffixLabel"));

    QVERIFY(prefixLabel != nullptr);
    QVERIFY(suffixLabel != nullptr);
    QVERIFY(prefixLabel->isVisible());
    QVERIFY(suffixLabel->isVisible());
    QVERIFY(widget.lineEdit()->geometry().width() < baseEditorWidth);
}

void tst_FilledTextField::inheritsPasswordEndActionBehavior()
{
    QtMaterial::QtMaterialFilledTextField widget;
    widget.setLabelText(QStringLiteral("Password"));
    widget.setEchoMode(QLineEdit::Password);
    widget.setText(QStringLiteral("secret"));
    realize(widget);

    QToolButton* endAction =
        requireChild<QToolButton>(widget, QStringLiteral("endActionButton"));

    QVERIFY(endAction != nullptr);
    QVERIFY(endAction->isVisible());
    QCOMPARE(widget.lineEdit()->echoMode(), QLineEdit::Password);

    QTest::mouseClick(endAction, Qt::LeftButton);
    QCoreApplication::processEvents();
    QCOMPARE(widget.lineEdit()->echoMode(), QLineEdit::Normal);
}

QTEST_MAIN(tst_FilledTextField)
#include "tst_filledtextfield.moc"