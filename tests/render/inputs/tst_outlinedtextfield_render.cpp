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

#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"

class tst_OutlinedTextFieldRender : public QObject
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
    void rendersRtlAccessoriesState();

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

void tst_OutlinedTextFieldRender::rendersDefaultState()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
    widget.setLabelText(QStringLiteral("Email"));
    widget.setSupportingText(QStringLiteral("We will not spam you"));

    const QImage image = captureWidget(widget);
    QVERIFY(!image.isNull());
    verifyOrUpdateBaseline(image, QStringLiteral("outlinedtextfield_default_light.png"));
}

void tst_OutlinedTextFieldRender::rendersFocusedState()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
    widget.setLabelText(QStringLiteral("Email"));
    widget.setSupportingText(QStringLiteral("We will not spam you"));
    widget.resize(qMax(widget.width(), 360), qMax(widget.height(), widget.sizeHint().height()));
    widget.show();

    QVERIFY(QTest::qWaitForWindowExposed(&widget));
    widget.setFocus(Qt::TabFocusReason);
    QVERIFY(QTest::qWaitFor([&widget]() {
        return widget.lineEdit() && widget.lineEdit()->hasFocus();
    }));

    const QImage image = widget.grab().toImage();
    QVERIFY(!image.isNull());
    verifyOrUpdateBaseline(image, QStringLiteral("outlinedtextfield_focused_light.png"));
}

void tst_OutlinedTextFieldRender::rendersErrorState()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
    widget.setLabelText(QStringLiteral("Email"));
    widget.setText(QStringLiteral("not-an-email"));
    widget.setErrorText(QStringLiteral("Invalid email"));
    widget.setHasErrorState(true);

    const QImage image = captureWidget(widget);
    QVERIFY(!image.isNull());
    verifyOrUpdateBaseline(image, QStringLiteral("outlinedtextfield_error_light.png"));
}

void tst_OutlinedTextFieldRender::rendersValidatorOnEditInvalidState()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
    QRegularExpressionValidator validator(
        QRegularExpression(QStringLiteral(R"(^[^\s@]+@[^\s@]+\.[^\s@]+$)")));

    widget.setLabelText(QStringLiteral("Email"));
    widget.setSupportingText(QStringLiteral("Enter a valid email"));
    widget.setErrorText(QStringLiteral("Invalid email"));
    widget.setValidator(&validator);
    widget.setValidationFeedbackMode(
        QtMaterial::QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnEdit);
    widget.setText(QStringLiteral("not-an-email"));

    QVERIFY(widget.hasAutomaticValidationError());
    QVERIFY(widget.hasErrorState());

    const QImage image = captureWidget(widget);
    QVERIFY(!image.isNull());
    verifyOrUpdateBaseline(image, QStringLiteral("outlinedtextfield_validator_on_edit_invalid_light.png"));
}

void tst_OutlinedTextFieldRender::rendersValidatorOnCommitInvalidState()
{
    QWidget host;
    host.resize(420, 180);

    auto* field = new QtMaterial::QtMaterialOutlinedTextField(&host);
    auto* other = new QPushButton(QStringLiteral("Other"), &host);

    field->setGeometry(20, 20, 360, 88);
    other->setGeometry(20, 120, 100, 30);

    QRegularExpressionValidator validator(
        QRegularExpression(QStringLiteral(R"(^[^\s@]+@[^\s@]+\.[^\s@]+$)")));

    field->setLabelText(QStringLiteral("Email"));
    field->setSupportingText(QStringLiteral("Enter a valid email"));
    field->setErrorText(QStringLiteral("Invalid email"));
    field->setValidator(&validator);
    field->setValidationFeedbackMode(
        QtMaterial::QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnCommit);
    field->setText(QStringLiteral("not-an-email"));

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
    verifyOrUpdateBaseline(image, QStringLiteral("outlinedtextfield_validator_on_commit_invalid_light.png"));
}

void tst_OutlinedTextFieldRender::rendersPrefixSuffixState()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
    widget.setLabelText(QStringLiteral("Website"));
    widget.setPrefixText(QStringLiteral("https://"));
    widget.setSuffixText(QStringLiteral(".com"));
    widget.setText(QStringLiteral("example"));
    widget.setSupportingText(QStringLiteral("Enter your public URL"));

    const QImage image = captureWidget(widget);
    QVERIFY(!image.isNull());
    verifyOrUpdateBaseline(image, QStringLiteral("outlinedtextfield_prefix_suffix_light.png"));
}

void tst_OutlinedTextFieldRender::rendersLeadingTrailingIconsState()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
    widget.setLabelText(QStringLiteral("Search"));
    widget.setText(QStringLiteral("material"));
    widget.setLeadingIcon(widget.style()->standardIcon(QStyle::SP_FileDialogContentsView));
    widget.setTrailingIcon(widget.style()->standardIcon(QStyle::SP_ArrowForward));
    widget.setSupportingText(QStringLiteral("Keyword lookup"));

    const QImage image = captureWidget(widget);
    QVERIFY(!image.isNull());
    verifyOrUpdateBaseline(image, QStringLiteral("outlinedtextfield_icons_light.png"));
}

void tst_OutlinedTextFieldRender::rendersPasswordToggleState()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
    widget.setLabelText(QStringLiteral("Password"));
    widget.setText(QStringLiteral("secret123"));
    widget.setEchoMode(QLineEdit::Password);
    widget.setEndActionMode(
        QtMaterial::QtMaterialOutlinedTextField::EndActionMode::TogglePasswordVisibility);
    widget.setSupportingText(QStringLiteral("At least 8 characters"));

    const QImage image = captureWidget(widget);
    QVERIFY(!image.isNull());
    verifyOrUpdateBaseline(image, QStringLiteral("outlinedtextfield_password_light.png"));
}

void tst_OutlinedTextFieldRender::rendersCustomTrailingActionState()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
    widget.setLabelText(QStringLiteral("Search"));
    widget.setText(QStringLiteral("material"));
    widget.setSupportingText(QStringLiteral("Run a query"));
    widget.setEndActionMode(
        QtMaterial::QtMaterialOutlinedTextField::EndActionMode::CustomTrailingAction);
    widget.setTrailingActionText(QStringLiteral("Go"));
    widget.setTrailingActionToolTip(QStringLiteral("Run search"));

    const QImage image = captureWidget(widget);
    QVERIFY(!image.isNull());
    verifyOrUpdateBaseline(image, QStringLiteral("outlinedtextfield_custom_trailing_action_light.png"));
}

void tst_OutlinedTextFieldRender::rendersRtlAccessoriesState()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
    widget.setLayoutDirection(Qt::RightToLeft);
    widget.setLabelText(QStringLiteral("Montant"));
    widget.setPrefixText(QStringLiteral("EUR"));
    widget.setSuffixText(QStringLiteral("TTC"));
    widget.setLeadingIcon(widget.style()->standardIcon(QStyle::SP_DriveHDIcon));
    widget.setText(QStringLiteral("125"));

    const QImage image = captureWidget(widget);
    QVERIFY(!image.isNull());
    verifyOrUpdateBaseline(image, QStringLiteral("outlinedtextfield_rtl_accessories_light.png"));
}

QTEST_MAIN(tst_OutlinedTextFieldRender)
#include "tst_outlinedtextfield_render.moc"