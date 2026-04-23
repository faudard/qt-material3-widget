#include <QtTest>
#include <QBuffer>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDir>
#include <QFileInfo>
#include <QImage>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QPushButton>

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

    verifyOrUpdateBaseline(
        captureWidget(widget),
        QStringLiteral("outlinedtextfield_default_light.png"));
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
    QVERIFY(QTest::qWaitFor([&widget]() { return widget.lineEdit() && widget.lineEdit()->hasFocus(); }));

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

    verifyOrUpdateBaseline(
        captureWidget(widget),
        QStringLiteral("outlinedtextfield_error_light.png"));
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

    verifyOrUpdateBaseline(
        captureWidget(widget),
        QStringLiteral("outlinedtextfield_validator_on_edit_invalid_light.png"));
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
    QVERIFY(QTest::qWaitFor([field]() { return field->lineEdit() && field->lineEdit()->hasFocus(); }));

    other->setFocus();
    QVERIFY(QTest::qWaitFor([other]() { return other->hasFocus(); }));
    QCoreApplication::processEvents();

    QVERIFY(field->hasAutomaticValidationError());
    QVERIFY(field->hasErrorState());

    const QImage image = field->grab().toImage();
    QVERIFY(!image.isNull());
    verifyOrUpdateBaseline(
        image,
        QStringLiteral("outlinedtextfield_validator_on_commit_invalid_light.png"));
}

QTEST_MAIN(tst_OutlinedTextFieldRender)
#include "tst_outlinedtextfield_render.moc"