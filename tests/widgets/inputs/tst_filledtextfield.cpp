#include <QtTest/QtTest>

#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QSignalSpy>
#include <QStyle>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>

#include "qtmaterial/widgets/inputs/qtmaterialfilledtextfield.h"
#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"

namespace {

template <typename T>
T* findChildByName(QWidget& parent, const QString& name)
{
    return parent.findChild<T*>(name);
}

bool realize(QWidget& widget)
{
    widget.resize(qMax(widget.width(), 360),
                  qMax(widget.height(), widget.sizeHint().height()));
    widget.show();

    if (!QTest::qWaitForWindowExposed(&widget)) {
        return false;
    }

    QCoreApplication::processEvents();
    return true;
}

QWidget* makeFocusHost(QtMaterial::QtMaterialFilledTextField*& field,
                       QPushButton*& other)
{
    QWidget* host = new QWidget;
    auto* layout = new QVBoxLayout(host);

    field = new QtMaterial::QtMaterialFilledTextField(host);
    other = new QPushButton(QStringLiteral("Other"), host);

    layout->addWidget(field);
    layout->addWidget(other);

    host->resize(420, 180);
    host->show();
    return host;
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
    void inheritsLeadingAndTrailingIconsBehavior();
    void inheritsPasswordEndActionBehavior();

    void customTrailingActionEmitsSignal();
    void customTrailingActionCanStayVisibleWhenEmpty();
    void clearActionHidesDecorativeTrailingIcon();
    void passwordRevealOverridesCustomTrailingAction();

    void manualOnlyIgnoresValidatorUntilManualError();
    void inheritsValidatorOnEditBehavior();
    void validatorOnCommitWaitsForCommitThenTracksFurtherEdits();
    void inheritsInputMaskValidationBehavior();
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

    QCOMPARE(widget.lineEdit()->accessibleDescription(),
             QStringLiteral("Required"));
}

void tst_FilledTextField::forwardsFocusToLineEdit()
{
    QtMaterial::QtMaterialFilledTextField widget;
    widget.setLabelText(QStringLiteral("Name"));

    QVERIFY(realize(widget));

    widget.setFocus(Qt::TabFocusReason);

    QTRY_VERIFY(widget.lineEdit() != nullptr);
    QTRY_VERIFY(widget.lineEdit()->hasFocus());
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

    const QSize first = widget.sizeHint();

    widget.setLabelText(
        QStringLiteral("Very long account display name label that should change width"));
    const QSize second = widget.sizeHint();

    QVERIFY(second.width() >= first.width());
}

void tst_FilledTextField::inheritsPrefixSuffixBehavior()
{
    QtMaterial::QtMaterialFilledTextField widget;
    widget.setLabelText(QStringLiteral("Price"));

    QVERIFY(realize(widget));

    const int baseEditorWidth = widget.lineEdit()->geometry().width();

    widget.setPrefixText(QStringLiteral("$"));
    widget.setSuffixText(QStringLiteral("USD"));
    QCoreApplication::processEvents();

    QLabel* prefixLabel = findChildByName<QLabel>(widget, QStringLiteral("prefixLabel"));
    QLabel* suffixLabel = findChildByName<QLabel>(widget, QStringLiteral("suffixLabel"));

    QVERIFY(prefixLabel != nullptr);
    QVERIFY(suffixLabel != nullptr);
    QVERIFY(prefixLabel->isVisible());
    QVERIFY(suffixLabel->isVisible());
    QVERIFY(widget.lineEdit()->geometry().width() < baseEditorWidth);
}

void tst_FilledTextField::inheritsLeadingAndTrailingIconsBehavior()
{
    QtMaterial::QtMaterialFilledTextField widget;
    widget.setLabelText(QStringLiteral("Search"));

    QVERIFY(realize(widget));

    const int baseEditorWidth = widget.lineEdit()->geometry().width();

    const QIcon leading = widget.style()->standardIcon(QStyle::SP_FileDialogContentsView);
    const QIcon trailing = widget.style()->standardIcon(QStyle::SP_ArrowForward);

    QVERIFY(!leading.isNull());
    QVERIFY(!trailing.isNull());

    widget.setLeadingIcon(leading);
    widget.setTrailingIcon(trailing);
    QCoreApplication::processEvents();

    QLabel* leadingLabel =
        findChildByName<QLabel>(widget, QStringLiteral("leadingIconLabel"));
    QLabel* trailingLabel =
        findChildByName<QLabel>(widget, QStringLiteral("trailingIconLabel"));

    QVERIFY(leadingLabel != nullptr);
    QVERIFY(trailingLabel != nullptr);

    QVERIFY(leadingLabel->isVisible());
    QVERIFY(trailingLabel->isVisible());
    QVERIFY(widget.lineEdit()->geometry().width() < baseEditorWidth);
}

void tst_FilledTextField::inheritsPasswordEndActionBehavior()
{
    QtMaterial::QtMaterialFilledTextField widget;
    widget.setLabelText(QStringLiteral("Password"));
    widget.setEchoMode(QLineEdit::Password);
    widget.setText(QStringLiteral("secret"));
    widget.setEndActionMode(
        QtMaterial::QtMaterialOutlinedTextField::EndActionMode::TogglePasswordVisibility);

    QVERIFY(realize(widget));

    QToolButton* endAction =
        findChildByName<QToolButton>(widget, QStringLiteral("endActionButton"));

    QVERIFY(endAction != nullptr);
    QVERIFY(endAction->isVisible());
    QCOMPARE(widget.lineEdit()->echoMode(), QLineEdit::Password);

    QSignalSpy spy(&widget, &QtMaterial::QtMaterialOutlinedTextField::passwordVisibilityChanged);

    QTest::mouseClick(endAction, Qt::LeftButton);
    QCoreApplication::processEvents();
    QCOMPARE(widget.lineEdit()->echoMode(), QLineEdit::Normal);
    QCOMPARE(spy.count(), 1);
}

void tst_FilledTextField::customTrailingActionEmitsSignal()
{
    QtMaterial::QtMaterialFilledTextField widget;
    widget.setLabelText(QStringLiteral("Search"));
    widget.setText(QStringLiteral("material"));
    widget.setEndActionMode(
        QtMaterial::QtMaterialOutlinedTextField::EndActionMode::CustomTrailingAction);
    widget.setTrailingActionText(QStringLiteral("Go"));
    widget.setTrailingActionToolTip(QStringLiteral("Run search"));

    QVERIFY(realize(widget));

    QToolButton* endAction =
        findChildByName<QToolButton>(widget, QStringLiteral("endActionButton"));

    QVERIFY(endAction != nullptr);
    QVERIFY(endAction->isVisible());
    QCOMPARE(endAction->text(), QStringLiteral("Go"));
    QCOMPARE(endAction->toolTip(), QStringLiteral("Run search"));

    QSignalSpy spy(&widget, &QtMaterial::QtMaterialOutlinedTextField::trailingActionTriggered);

    QTest::mouseClick(endAction, Qt::LeftButton);
    QCoreApplication::processEvents();

    QCOMPARE(spy.count(), 1);
}

void tst_FilledTextField::customTrailingActionCanStayVisibleWhenEmpty()
{
    QtMaterial::QtMaterialFilledTextField widget;
    widget.setLabelText(QStringLiteral("Filter"));
    widget.setEndActionMode(
        QtMaterial::QtMaterialOutlinedTextField::EndActionMode::CustomTrailingAction);
    widget.setTrailingActionText(QStringLiteral("Open"));
    widget.setTrailingActionVisibleWhenEmpty(true);

    QVERIFY(realize(widget));

    QToolButton* endAction =
        findChildByName<QToolButton>(widget, QStringLiteral("endActionButton"));

    QVERIFY(endAction != nullptr);
    QVERIFY(endAction->isVisible());
    QCOMPARE(endAction->text(), QStringLiteral("Open"));
}

void tst_FilledTextField::clearActionHidesDecorativeTrailingIcon()
{
    QtMaterial::QtMaterialFilledTextField widget;
    widget.setLabelText(QStringLiteral("Query"));
    widget.setText(QStringLiteral("abc"));
    widget.setTrailingIcon(widget.style()->standardIcon(QStyle::SP_ArrowForward));
    widget.setEndActionMode(
        QtMaterial::QtMaterialOutlinedTextField::EndActionMode::ClearText);
    widget.setClearButtonEnabled(true);

    QVERIFY(realize(widget));

    QLabel* trailingLabel =
        findChildByName<QLabel>(widget, QStringLiteral("trailingIconLabel"));
    QToolButton* endAction =
        findChildByName<QToolButton>(widget, QStringLiteral("endActionButton"));

    QVERIFY(trailingLabel != nullptr);
    QVERIFY(endAction != nullptr);

    QVERIFY(endAction->isVisible());
    QVERIFY(!trailingLabel->isVisible());

    QSignalSpy spy(&widget, &QtMaterial::QtMaterialOutlinedTextField::clearTriggered);

    QTest::mouseClick(endAction, Qt::LeftButton);
    QCoreApplication::processEvents();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(widget.text(), QString());
}

void tst_FilledTextField::passwordRevealOverridesCustomTrailingAction()
{
    QtMaterial::QtMaterialFilledTextField widget;
    widget.setLabelText(QStringLiteral("Password"));
    widget.setText(QStringLiteral("secret"));
    widget.setEchoMode(QLineEdit::Password);
    widget.setEndActionMode(
        QtMaterial::QtMaterialOutlinedTextField::EndActionMode::TogglePasswordVisibility);
    widget.setTrailingActionText(QStringLiteral("Go"));
    widget.setTrailingActionToolTip(QStringLiteral("Should not be used"));

    QVERIFY(realize(widget));

    QToolButton* endAction =
        findChildByName<QToolButton>(widget, QStringLiteral("endActionButton"));

    QVERIFY(endAction != nullptr);
    QVERIFY(endAction->isVisible());
    QVERIFY(endAction->text() == QStringLiteral("Show")
            || endAction->text() == QStringLiteral("Hide"));
    QVERIFY(endAction->text() != QStringLiteral("Go"));
}

void tst_FilledTextField::manualOnlyIgnoresValidatorUntilManualError()
{
    QtMaterial::QtMaterialFilledTextField widget;
    QRegularExpressionValidator validator(
        QRegularExpression(QStringLiteral(R"(^\d{4}$)")));

    widget.setValidator(&validator);
    widget.setValidationFeedbackMode(
        QtMaterial::QtMaterialOutlinedTextField::ValidationFeedbackMode::ManualOnly);
    widget.setText(QStringLiteral("12"));

    QVERIFY(!widget.hasAutomaticValidationError());
    QVERIFY(!widget.hasErrorState());

    widget.setHasErrorState(true);
    QVERIFY(widget.hasErrorState());

    widget.setHasErrorState(false);
    QVERIFY(!widget.hasErrorState());
}

void tst_FilledTextField::inheritsValidatorOnEditBehavior()
{
    QtMaterial::QtMaterialFilledTextField widget;
    QRegularExpressionValidator validator(
        QRegularExpression(QStringLiteral(R"(^[A-Z]{3}$)")));

    widget.setValidator(&validator);
    widget.setValidationFeedbackMode(
        QtMaterial::QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnEdit);

    widget.setText(QStringLiteral("ab"));
    QCoreApplication::processEvents();

    QVERIFY(widget.hasAutomaticValidationError());
    QVERIFY(widget.hasErrorState());

    widget.setText(QStringLiteral("ABC"));
    QCoreApplication::processEvents();

    QVERIFY(!widget.hasAutomaticValidationError());
    QVERIFY(!widget.hasErrorState());
}

void tst_FilledTextField::validatorOnCommitWaitsForCommitThenTracksFurtherEdits()
{
    QtMaterial::QtMaterialFilledTextField* field = nullptr;
    QPushButton* other = nullptr;
    std::unique_ptr<QWidget> host(makeFocusHost(field, other));

    QVERIFY(host != nullptr);
    QVERIFY(field != nullptr);
    QVERIFY(other != nullptr);
    QVERIFY(QTest::qWaitForWindowExposed(host.get()));

    QRegularExpressionValidator validator(
        QRegularExpression(QStringLiteral(R"(^[A-Z]{3}$)")));

    field->setValidator(&validator);
    field->setValidationFeedbackMode(
        QtMaterial::QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnCommit);

    field->lineEdit()->setFocus();
    QTRY_VERIFY(field->lineEdit()->hasFocus());

    field->setText(QStringLiteral("ab"));
    QCoreApplication::processEvents();

    QVERIFY(!field->hasAutomaticValidationError());
    QVERIFY(!field->hasErrorState());

    other->setFocus();
    QTRY_VERIFY(other->hasFocus());
    QCoreApplication::processEvents();

    QVERIFY(field->hasAutomaticValidationError());
    QVERIFY(field->hasErrorState());

    field->lineEdit()->setFocus();
    QTRY_VERIFY(field->lineEdit()->hasFocus());

    field->setText(QStringLiteral("ABC"));
    QCoreApplication::processEvents();

    QVERIFY(!field->hasAutomaticValidationError());
    QVERIFY(!field->hasErrorState());
}

void tst_FilledTextField::inheritsInputMaskValidationBehavior()
{
    QtMaterial::QtMaterialFilledTextField widget;

    widget.setInputMask(QStringLiteral("000-000"));
    widget.setValidationFeedbackMode(
        QtMaterial::QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnEdit);

    widget.setText(QStringLiteral("12"));
    QCoreApplication::processEvents();

    QVERIFY(widget.hasAutomaticValidationError());
    QVERIFY(widget.hasErrorState());

    widget.setText(QStringLiteral("123456"));
    QCoreApplication::processEvents();

    QVERIFY(!widget.hasAutomaticValidationError());
    QVERIFY(!widget.hasErrorState());
}

QTEST_MAIN(tst_FilledTextField)
#include "tst_filledtextfield.moc"