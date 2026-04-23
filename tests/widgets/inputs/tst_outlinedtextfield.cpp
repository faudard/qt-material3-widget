#include <QtTest/QtTest>

#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QStyle>
#include <QToolButton>
#include <QLineEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QVBoxLayout>
#include <QWidget>

#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"

namespace {

template <typename T>
T* requireChild(QWidget& parent, const QString& name)
{
    T* child = parent.findChild<T*>(name);
    return child;
}

void realize(QWidget& widget)
{
    widget.resize(qMax(widget.width(), 360), qMax(widget.height(), widget.sizeHint().height()));
    widget.show();
    QTest::qWait(1);
    QCoreApplication::processEvents();
}

} // namespace

class tst_OutlinedTextField : public QObject
{
    Q_OBJECT

private slots:
    void basicConstruction();
    void accessibilityState();
    void forwardsFocusToLineEdit();
    void sizeHintRespectsShellHeight();
    void contentChangeInvalidatesShellText();

    void placeholderTextPassthrough();
    void prefixAndSuffixReserveEditorSpace();
    void leadingAndTrailingIconsReserveEditorSpace();
    void rtlMirrorsAccessoryLayout();
    void passwordEndActionTogglesLineEditEchoMode();
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

void tst_OutlinedTextField::placeholderTextPassthrough()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
    widget.setPlaceholderText(QStringLiteral("name@example.com"));

    QCOMPARE(widget.placeholderText(), QStringLiteral("name@example.com"));
    QVERIFY(widget.lineEdit() != nullptr);
    QCOMPARE(widget.lineEdit()->placeholderText(), QStringLiteral("name@example.com"));
}

void tst_OutlinedTextField::prefixAndSuffixReserveEditorSpace()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
    widget.setLabelText(QStringLiteral("Website"));
    realize(widget);

    const int baseEditorWidth = widget.lineEdit()->geometry().width();

    widget.setPrefixText(QStringLiteral("https://"));
    widget.setSuffixText(QStringLiteral(".com"));
    QCoreApplication::processEvents();

    QLabel* prefixLabel = requireChild<QLabel>(widget, QStringLiteral("prefixLabel"));
    QLabel* suffixLabel = requireChild<QLabel>(widget, QStringLiteral("suffixLabel"));

    QVERIFY(prefixLabel != nullptr);
    QVERIFY(suffixLabel != nullptr);

    QVERIFY(prefixLabel->isVisible());
    QVERIFY(suffixLabel->isVisible());
    QVERIFY(!prefixLabel->text().isEmpty());
    QVERIFY(!suffixLabel->text().isEmpty());
    QVERIFY(!prefixLabel->geometry().isEmpty());
    QVERIFY(!suffixLabel->geometry().isEmpty());

    QVERIFY(widget.lineEdit()->geometry().width() < baseEditorWidth);
    QVERIFY(prefixLabel->geometry().right() < widget.lineEdit()->geometry().left()
            || prefixLabel->geometry().left() > widget.lineEdit()->geometry().right());
    QVERIFY(suffixLabel->geometry().right() < widget.lineEdit()->geometry().left()
            || suffixLabel->geometry().left() > widget.lineEdit()->geometry().right());
}

void tst_OutlinedTextField::leadingAndTrailingIconsReserveEditorSpace()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
    widget.setLabelText(QStringLiteral("Search"));
    realize(widget);

    const int baseEditorWidth = widget.lineEdit()->geometry().width();

    const QIcon leading = widget.style()->standardIcon(QStyle::SP_FileDialogStart);
    const QIcon trailing = widget.style()->standardIcon(QStyle::SP_DialogOpenButton);

    QVERIFY(!leading.isNull());
    QVERIFY(!trailing.isNull());

    widget.setLeadingIcon(leading);
    widget.setTrailingIcon(trailing);
    QCoreApplication::processEvents();

    QLabel* leadingLabel = requireChild<QLabel>(widget, QStringLiteral("leadingIconLabel"));
    QLabel* trailingLabel = requireChild<QLabel>(widget, QStringLiteral("trailingIconLabel"));

    QVERIFY(leadingLabel != nullptr);
    QVERIFY(trailingLabel != nullptr);

    QVERIFY(leadingLabel->isVisible());
    QVERIFY(trailingLabel->isVisible());
    QVERIFY(!leadingLabel->geometry().isEmpty());
    QVERIFY(!trailingLabel->geometry().isEmpty());

    QVERIFY(widget.lineEdit()->geometry().width() < baseEditorWidth);
}

void tst_OutlinedTextField::rtlMirrorsAccessoryLayout()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
    widget.setLabelText(QStringLiteral("Amount"));
    widget.setLeadingIcon(widget.style()->standardIcon(QStyle::SP_ArrowRight));
    widget.setPrefixText(QStringLiteral("EUR"));
    widget.resize(360, widget.sizeHint().height());

    widget.setLayoutDirection(Qt::LeftToRight);
    realize(widget);

    QLabel* leadingLabel = requireChild<QLabel>(widget, QStringLiteral("leadingIconLabel"));
    QLabel* prefixLabel = requireChild<QLabel>(widget, QStringLiteral("prefixLabel"));

    QVERIFY(leadingLabel != nullptr);
    QVERIFY(prefixLabel != nullptr);

    const QRect ltrEditor = widget.lineEdit()->geometry();
    const QRect ltrLeading = leadingLabel->geometry();
    const QRect ltrPrefix = prefixLabel->geometry();

    QVERIFY(ltrLeading.right() <= ltrEditor.left());
    QVERIFY(ltrPrefix.right() <= ltrEditor.left());

    widget.setLayoutDirection(Qt::RightToLeft);
    QCoreApplication::processEvents();

    const QRect rtlEditor = widget.lineEdit()->geometry();
    const QRect rtlLeading = leadingLabel->geometry();
    const QRect rtlPrefix = prefixLabel->geometry();

    QVERIFY(rtlLeading.left() >= rtlEditor.right());
    QVERIFY(rtlPrefix.left() >= rtlEditor.right());
}

void tst_OutlinedTextField::passwordEndActionTogglesLineEditEchoMode()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
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

    QTest::mouseClick(endAction, Qt::LeftButton);
    QCoreApplication::processEvents();
    QCOMPARE(widget.lineEdit()->echoMode(), QLineEdit::Password);
}

QTEST_MAIN(tst_OutlinedTextField)
#include "tst_outlinedtextfield.moc"