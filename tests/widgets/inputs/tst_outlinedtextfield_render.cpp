#include <QtTest>
#include <QBuffer>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDir>
#include <QFileInfo>
#include <QImage>
#include <QStyle>

#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"

class tst_OutlinedTextFieldRender : public QObject
{
    Q_OBJECT

private slots:
    void rendersDefaultState();
    void rendersErrorState();
    void rendersPrefixSuffixState();
    void rendersLeadingTrailingIconsState();
    void rendersPasswordToggleState();
    void rendersRtlAccessoryState();

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
        QVERIFY(QTest::qWaitForWindowExposed(&widget));
        QCoreApplication::processEvents();
        const QImage image = widget.grab().toImage();
        QVERIFY(!image.isNull());
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

void tst_OutlinedTextFieldRender::rendersPrefixSuffixState()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
    widget.setLabelText(QStringLiteral("Website"));
    widget.setPrefixText(QStringLiteral("https://"));
    widget.setSuffixText(QStringLiteral(".com"));
    widget.setText(QStringLiteral("example"));
    widget.setSupportingText(QStringLiteral("Enter your public URL"));

    verifyOrUpdateBaseline(
        captureWidget(widget),
        QStringLiteral("outlinedtextfield_prefix_suffix_light.png"));
}

void tst_OutlinedTextFieldRender::rendersLeadingTrailingIconsState()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
    widget.setLabelText(QStringLiteral("Search"));
    widget.setText(QStringLiteral("material"));
    widget.setLeadingIcon(widget.style()->standardIcon(QStyle::SP_FileDialogContentsView));
    widget.setTrailingIcon(widget.style()->standardIcon(QStyle::SP_ArrowForward));
    widget.setSupportingText(QStringLiteral("Keyword lookup"));

    verifyOrUpdateBaseline(
        captureWidget(widget),
        QStringLiteral("outlinedtextfield_icons_light.png"));
}

void tst_OutlinedTextFieldRender::rendersPasswordToggleState()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
    widget.setLabelText(QStringLiteral("Password"));
    widget.setText(QStringLiteral("secret123"));
    widget.setEchoMode(QLineEdit::Password);
    widget.setSupportingText(QStringLiteral("At least 8 characters"));

    verifyOrUpdateBaseline(
        captureWidget(widget),
        QStringLiteral("outlinedtextfield_password_light.png"));
}

void tst_OutlinedTextFieldRender::rendersRtlAccessoryState()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
    widget.setLayoutDirection(Qt::RightToLeft);
    widget.setLabelText(QStringLiteral("Montant"));
    widget.setPrefixText(QStringLiteral("EUR"));
    widget.setSuffixText(QStringLiteral("TTC"));
    widget.setLeadingIcon(widget.style()->standardIcon(QStyle::SP_DriveHDIcon));
    widget.setText(QStringLiteral("125"));

    verifyOrUpdateBaseline(
        captureWidget(widget),
        QStringLiteral("outlinedtextfield_rtl_accessories_light.png"));
}

QTEST_MAIN(tst_OutlinedTextFieldRender)
#include "tst_outlinedtextfield_render.moc"