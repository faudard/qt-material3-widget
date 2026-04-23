#include <QtTest>
#include <QBuffer>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDir>
#include <QFileInfo>
#include <QImage>
#include <QPushButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QStyle>

#include "qtmaterial/widgets/inputs/qtmaterialfilledtextfield.h"
#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"

class tst_FilledTextFieldRender : public QObject
{
    Q_OBJECT

private slots:
    void rendersDefaultState();
    void rendersFocusedState();
    void rendersErrorState();
    void rendersValidatorOnEditInvalidState();
    void rendersValidatorOnCommitInvalidState();

    void rendersPrefixSuffixState();
    void rendersLeadingTrailingIconsState();
    void rendersPasswordToggleState();
    void rendersCustomTrailingActionState();

private:
    static QByteArray imageHash(const QImage& image)
    {
        QByteArray bytes;
        QBuffer buffer(&bytes);
        buffer.open(QIODevice::WriteOnly);
        image.save(&buffer, "PNG");
        return QCryptographicHash::hash(bytes, QCryptographicHash::Sha256).toHex();
    }

    static QString baselinePath(const QString& name)
    {
        return QDir(QCoreApplication::applicationDirPath()).absoluteFilePath(
            QStringLiteral("../../../tools/render-baselines/inputs/") + name);
    }

    static void verifyOrUpdateBaseline(const QImage& image, const QString& name)
    {
        const QString baseline = baselinePath(name);
        const QByteArray update = qgetenv("QTMATERIAL3_UPDATE_BASELINES");

        if (!update.isEmpty()) {
            QDir().mkpath(QFileInfo(baseline).absolutePath());
            QVERIFY(image.save(baseline));
            return;
        }

        if (!QFileInfo::exists(baseline)) {
            QSKIP(qPrintable(QStringLiteral("Missing render baseline: %1").arg(baseline)));
        }

        const QImage baselineImage(baseline);
        QVERIFY(!baselineImage.isNull());
        QCOMPARE(image.size(), baselineImage.size());
        QCOMPARE(imageHash(image), imageHash(baselineImage));
    }

    static QImage captureWidget(QWidget& widget)
    {
        widget.resize(qMax(widget.width(), 360), qMax(widget.height(), widget.sizeHint().height()));
        widget.show();

        if (!QTest::qWaitForWindowExposed(&widget)) {
            qWarning("Window was not exposed in captureWidget");
            return QImage();
        }

        QCoreApplication::processEvents();

        const QImage image = widget.grab().toImage();
        if (image.isNull()) {
            qWarning("Grabbed image is null in captureWidget");
            return QImage();
        }

        return image;
    }
};

void tst_FilledTextFieldRender::rendersDefaultState()
{
    QtMaterial::QtMaterialFilledTextField widget;
    widget.setLabelText(QStringLiteral("Name"));
    widget.setSupportingText(QStringLiteral("Shown on your profile"));

    const QImage image = captureWidget(widget);
    QVERIFY(!image.isNull());
    verifyOrUpdateBaseline(image, QStringLiteral("filledtextfield_default_light.png"));
}

void tst_FilledTextFieldRender::rendersFocusedState()
{
    QtMaterial::QtMaterialFilledTextField widget;
    widget.setLabelText(QStringLiteral("Name"));
    widget.resize(qMax(widget.width(), 360), qMax(widget.height(), widget.sizeHint().height()));
    widget.show();

    QVERIFY(QTest::qWaitForWindowExposed(&widget));
    widget.setFocus(Qt::TabFocusReason);
    QVERIFY(QTest::qWaitFor([&widget]() {
        return widget.lineEdit() && widget.lineEdit()->hasFocus();
    }));

    const QImage image = widget.grab().toImage();
    QVERIFY(!image.isNull());
    verifyOrUpdateBaseline(image, QStringLiteral("filledtextfield_focused_light.png"));
}

void tst_FilledTextFieldRender::rendersErrorState()
{
    QtMaterial::QtMaterialFilledTextField widget;
    widget.setLabelText(QStringLiteral("Name"));
    widget.setText(QStringLiteral(""));
    widget.setErrorText(QStringLiteral("Required"));
    widget.setHasErrorState(true);

    const QImage image = captureWidget(widget);
    QVERIFY(!image.isNull());
    verifyOrUpdateBaseline(image, QStringLiteral("filledtextfield_error_light.png"));
}

void tst_FilledTextFieldRender::rendersValidatorOnEditInvalidState()
{
    QtMaterial::QtMaterialFilledTextField widget;
    QRegularExpressionValidator validator(
        QRegularExpression(QStringLiteral(R"(^[A-Z]{3}$)")));

    widget.setLabelText(QStringLiteral("Code"));
    widget.setSupportingText(QStringLiteral("Use a 3-letter code"));
    widget.setErrorText(QStringLiteral("Invalid code"));
    widget.setValidator(&validator);
    widget.setValidationFeedbackMode(
        QtMaterial::QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnEdit);
    widget.setText(QStringLiteral("ab"));

    QVERIFY(widget.hasAutomaticValidationError());
    QVERIFY(widget.hasErrorState());

    const QImage image = captureWidget(widget);
    QVERIFY(!image.isNull());
    verifyOrUpdateBaseline(image, QStringLiteral("filledtextfield_validator_on_edit_invalid_light.png"));
}

void tst_FilledTextFieldRender::rendersValidatorOnCommitInvalidState()
{
    QWidget host;
    host.resize(420, 180);

    auto* field = new QtMaterial::QtMaterialFilledTextField(&host);
    auto* other = new QPushButton(QStringLiteral("Other"), &host);

    field->setGeometry(20, 20, 360, 88);
    other->setGeometry(20, 120, 100, 30);

    QRegularExpressionValidator validator(
        QRegularExpression(QStringLiteral(R"(^[A-Z]{3}$)")));

    field->setLabelText(QStringLiteral("Code"));
    field->setSupportingText(QStringLiteral("Use a 3-letter code"));
    field->setErrorText(QStringLiteral("Invalid code"));
    field->setValidator(&validator);
    field->setValidationFeedbackMode(
        QtMaterial::QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnCommit);
    field->setText(QStringLiteral("ab"));

    host.show();
    QVERIFY(QTest::qWaitForWindowExposed(&host));

    field->lineEdit()->setFocus();
    QVERIFY(QTest::qWaitFor([field]() {
        return field->lineEdit() && field->lineEdit()->hasFocus();
    }));

    other->setFocus();
    QVERIFY(QTest::qWaitFor([other]() { return other->hasFocus(); }));
    QCoreApplication::processEvents();

    QVERIFY(field->hasAutomaticValidationError());
    QVERIFY(field->hasErrorState());

    const QImage image = field->grab().toImage();
    QVERIFY(!image.isNull());
    verifyOrUpdateBaseline(image, QStringLiteral("filledtextfield_validator_on_commit_invalid_light.png"));
}

void tst_FilledTextFieldRender::rendersPrefixSuffixState()
{
    QtMaterial::QtMaterialFilledTextField widget;
    widget.setLabelText(QStringLiteral("Price"));
    widget.setPrefixText(QStringLiteral("$"));
    widget.setSuffixText(QStringLiteral("USD"));
    widget.setText(QStringLiteral("149"));
    widget.setSupportingText(QStringLiteral("Taxes excluded"));

    const QImage image = captureWidget(widget);
    QVERIFY(!image.isNull());
    verifyOrUpdateBaseline(image, QStringLiteral("filledtextfield_prefix_suffix_light.png"));
}

void tst_FilledTextFieldRender::rendersLeadingTrailingIconsState()
{
    QtMaterial::QtMaterialFilledTextField widget;
    widget.setLabelText(QStringLiteral("Search"));
    widget.setText(QStringLiteral("dashboard"));
    widget.setLeadingIcon(widget.style()->standardIcon(QStyle::SP_FileDialogContentsView));
    widget.setTrailingIcon(widget.style()->standardIcon(QStyle::SP_ArrowForward));

    const QImage image = captureWidget(widget);
    QVERIFY(!image.isNull());
    verifyOrUpdateBaseline(image, QStringLiteral("filledtextfield_icons_light.png"));
}

void tst_FilledTextFieldRender::rendersPasswordToggleState()
{
    QtMaterial::QtMaterialFilledTextField widget;
    widget.setLabelText(QStringLiteral("Password"));
    widget.setEchoMode(QLineEdit::Password);
    widget.setEndActionMode(
        QtMaterial::QtMaterialOutlinedTextField::EndActionMode::TogglePasswordVisibility);
    widget.setText(QStringLiteral("secret123"));
    widget.setSupportingText(QStringLiteral("Use a strong password"));

    const QImage image = captureWidget(widget);
    QVERIFY(!image.isNull());
    verifyOrUpdateBaseline(image, QStringLiteral("filledtextfield_password_light.png"));
}

void tst_FilledTextFieldRender::rendersCustomTrailingActionState()
{
    QtMaterial::QtMaterialFilledTextField widget;
    widget.setLabelText(QStringLiteral("Search"));
    widget.setText(QStringLiteral("material"));
    widget.setSupportingText(QStringLiteral("Run a query"));
    widget.setEndActionMode(
        QtMaterial::QtMaterialOutlinedTextField::EndActionMode::CustomTrailingAction);
    widget.setTrailingActionText(QStringLiteral("Go"));
    widget.setTrailingActionToolTip(QStringLiteral("Run search"));

    const QImage image = captureWidget(widget);
    QVERIFY(!image.isNull());
    verifyOrUpdateBaseline(image, QStringLiteral("filledtextfield_custom_trailing_action_light.png"));
}

QTEST_MAIN(tst_FilledTextFieldRender)
#include "tst_filledtextfield_render.moc"