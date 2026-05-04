#include <QtTest/QtTest>

#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QSignalSpy>
#include <QToolButton>

#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"

namespace {

void polishAndProcess(QWidget& widget)
{
    widget.ensurePolished();
    widget.resize(qMax(widget.sizeHint().width(), 360), widget.sizeHint().height());
    widget.show();
    QCoreApplication::processEvents();
}

QIcon testIcon()
{
    QPixmap pixmap(18, 18);
    pixmap.fill(Qt::black);
    return QIcon(pixmap);
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
    void placeholderRoundTrip();
    void prefixAndSuffixRoundTripAndCreateVisibleLabels();
    void leadingAndTrailingIconRoundTripAndCreateVisibleLabels();
    void clearButtonClearsTextAndEmitsSignal();
    void passwordToggleChangesEchoModeAndEmitsSignal();
    void customTrailingActionEmitsSignal();
    void validatorOnEditSetsAutomaticError();
    void manualErrorStateIsIndependentFromAutomaticValidation();
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

void tst_OutlinedTextField::placeholderRoundTrip()
{
    QtMaterial::QtMaterialOutlinedTextField widget;

    widget.setPlaceholderText(QStringLiteral("name@example.com"));

    QCOMPARE(widget.placeholderText(), QStringLiteral("name@example.com"));
    QVERIFY(widget.lineEdit() != nullptr);
    QCOMPARE(widget.lineEdit()->placeholderText(), QStringLiteral("name@example.com"));
}

void tst_OutlinedTextField::prefixAndSuffixRoundTripAndCreateVisibleLabels()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
    widget.setLabelText(QStringLiteral("Amount"));
    widget.setPrefixText(QStringLiteral("€"));
    widget.setSuffixText(QStringLiteral("EUR"));

    polishAndProcess(widget);

    QCOMPARE(widget.prefixText(), QStringLiteral("€"));
    QCOMPARE(widget.suffixText(), QStringLiteral("EUR"));

    auto* prefix = widget.findChild<QLabel*>(QStringLiteral("prefixLabel"));
    auto* suffix = widget.findChild<QLabel*>(QStringLiteral("suffixLabel"));

    QVERIFY(prefix != nullptr);
    QVERIFY(suffix != nullptr);
    QCOMPARE(prefix->text(), QStringLiteral("€"));
    QCOMPARE(suffix->text(), QStringLiteral("EUR"));
    QVERIFY(!prefix->isHidden());
    QVERIFY(!suffix->isHidden());
    QVERIFY(prefix->geometry().isValid());
    QVERIFY(suffix->geometry().isValid());
    QVERIFY(widget.lineEdit()->geometry().isValid());
    QVERIFY(widget.lineEdit()->geometry().left() > widget.rect().left());
}

void tst_OutlinedTextField::leadingAndTrailingIconRoundTripAndCreateVisibleLabels()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
    const QIcon leading = testIcon();
    const QIcon trailing = testIcon();

    widget.setLeadingIcon(leading);
    widget.setTrailingIcon(trailing);

    polishAndProcess(widget);

    QVERIFY(!widget.leadingIcon().isNull());
    QVERIFY(!widget.trailingIcon().isNull());

    auto* leadingLabel = widget.findChild<QLabel*>(QStringLiteral("leadingIconLabel"));
    auto* trailingLabel = widget.findChild<QLabel*>(QStringLiteral("trailingIconLabel"));

    QVERIFY(leadingLabel != nullptr);
    QVERIFY(trailingLabel != nullptr);
    QVERIFY(!leadingLabel->isHidden());
    QVERIFY(!trailingLabel->isHidden());
    QVERIFY(leadingLabel->geometry().isValid());
    QVERIFY(trailingLabel->geometry().isValid());
}

void tst_OutlinedTextField::clearButtonClearsTextAndEmitsSignal()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
    widget.setClearButtonEnabled(true);
    widget.setEndActionMode(QtMaterial::QtMaterialOutlinedTextField::EndActionMode::ClearText);
    widget.setText(QStringLiteral("abc"));

    polishAndProcess(widget);

    auto* button = widget.findChild<QToolButton*>(QStringLiteral("endActionButton"));
    QVERIFY(button != nullptr);
    QVERIFY(!button->isHidden());
    QVERIFY(button->geometry().isValid());

    QSignalSpy spy(&widget, &QtMaterial::QtMaterialOutlinedTextField::clearTriggered);
    QTest::mouseClick(button, Qt::LeftButton);

    QCOMPARE(widget.text(), QString());
    QCOMPARE(spy.count(), 1);
}

void tst_OutlinedTextField::passwordToggleChangesEchoModeAndEmitsSignal()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
    widget.setEchoMode(QLineEdit::Password);
    widget.setEndActionMode(QtMaterial::QtMaterialOutlinedTextField::EndActionMode::TogglePasswordVisibility);
    widget.setText(QStringLiteral("secret"));

    polishAndProcess(widget);

    auto* button = widget.findChild<QToolButton*>(QStringLiteral("endActionButton"));
    QVERIFY(button != nullptr);
    QVERIFY(!button->isHidden());
    QCOMPARE(widget.echoMode(), QLineEdit::Password);
    QCOMPARE(widget.lineEdit()->echoMode(), QLineEdit::Password);

    QSignalSpy spy(&widget, &QtMaterial::QtMaterialOutlinedTextField::passwordVisibilityChanged);
    QTest::mouseClick(button, Qt::LeftButton);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().at(0).toBool(), true);
    QCOMPARE(widget.echoMode(), QLineEdit::Password);
    QCOMPARE(widget.lineEdit()->echoMode(), QLineEdit::Normal);
}

void tst_OutlinedTextField::customTrailingActionEmitsSignal()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
    widget.setEndActionMode(QtMaterial::QtMaterialOutlinedTextField::EndActionMode::CustomTrailingAction);
    widget.setTrailingActionText(QStringLiteral("Go"));
    widget.setTrailingActionToolTip(QStringLiteral("Run action"));
    widget.setTrailingActionVisibleWhenEmpty(true);

    polishAndProcess(widget);

    auto* button = widget.findChild<QToolButton*>(QStringLiteral("endActionButton"));
    QVERIFY(button != nullptr);
    QVERIFY(!button->isHidden());
    QCOMPARE(button->text(), QStringLiteral("Go"));
    QCOMPARE(button->toolTip(), QStringLiteral("Run action"));

    QSignalSpy spy(&widget, &QtMaterial::QtMaterialOutlinedTextField::trailingActionTriggered);
    QTest::mouseClick(button, Qt::LeftButton);

    QCOMPARE(spy.count(), 1);
}

void tst_OutlinedTextField::validatorOnEditSetsAutomaticError()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
    QRegularExpressionValidator validator(QRegularExpression(QStringLiteral("^[0-9]{3}$")), &widget);

    widget.setValidator(&validator);
    widget.setValidationFeedbackMode(
        QtMaterial::QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnEdit);

    widget.setText(QStringLiteral("abc"));

    QVERIFY(widget.hasAutomaticValidationError());
    QVERIFY(widget.hasErrorState());

    widget.setText(QStringLiteral("123"));

    QVERIFY(!widget.hasAutomaticValidationError());
    QVERIFY(!widget.hasErrorState());
}

void tst_OutlinedTextField::manualErrorStateIsIndependentFromAutomaticValidation()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
    QRegularExpressionValidator validator(QRegularExpression(QStringLiteral("^[0-9]{3}$")), &widget);

    widget.setValidator(&validator);
    widget.setValidationFeedbackMode(
        QtMaterial::QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnEdit);

    widget.setText(QStringLiteral("abc"));
    QVERIFY(widget.hasAutomaticValidationError());
    QVERIFY(widget.hasErrorState());

    widget.setHasErrorState(true);
    widget.setText(QStringLiteral("123"));

    QVERIFY(!widget.hasAutomaticValidationError());
    QVERIFY(widget.hasErrorState());

    widget.setHasErrorState(false);
    QVERIFY(!widget.hasErrorState());
}

QTEST_MAIN(tst_OutlinedTextField)
#include "tst_outlinedtextfield.moc"
