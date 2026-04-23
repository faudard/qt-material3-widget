#include <QtTest>
#include <QBuffer>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDir>
#include <QFileInfo>
#include <QImage>
#include <QStyle>

#include "qtmaterial/widgets/inputs/qtmaterialfilledtextfield.h"

class tst_FilledTextFieldRender : public QObject
{
    Q_OBJECT

private slots:
    void rendersDefaultState();
    void rendersFocusedState();
    void rendersErrorState();
    void rendersPrefixSuffixState();
    void rendersLeadingTrailingIconsState();
    void rendersPasswordToggleState();

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

void tst_FilledTextFieldRender::rendersDefaultState()
{
    QtMaterial::QtMaterialFilledTextField widget;
    widget.setLabelText(QStringLiteral("Name"));
    widget.setSupportingText(QStringLiteral("Shown on your profile"));

    verifyOrUpdateBaseline(
        captureWidget(widget),
        QStringLiteral("filledtextfield_default_light.png"));
}

void tst_FilledTextFieldRender::rendersFocusedState()
{
    QtMaterial::QtMaterialFilledTextField widget;
    widget.setLabelText(QStringLiteral("Name"));
    widget.resize(360, widget.sizeHint().height());
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

    verifyOrUpdateBaseline(
        captureWidget(widget),
        QStringLiteral("filledtextfield_error_light.png"));
}

void tst_FilledTextFieldRender::rendersPrefixSuffixState()
{
    QtMaterial::QtMaterialFilledTextField widget;
    widget.setLabelText(QStringLiteral("Price"));
    widget.setPrefixText(QStringLiteral("$"));
    widget.setSuffixText(QStringLiteral("USD"));
    widget.setText(QStringLiteral("149"));
    widget.setSupportingText(QStringLiteral("Taxes excluded"));

    verifyOrUpdateBaseline(
        captureWidget(widget),
        QStringLiteral("filledtextfield_prefix_suffix_light.png"));
}

void tst_FilledTextFieldRender::rendersLeadingTrailingIconsState()
{
    QtMaterial::QtMaterialFilledTextField widget;
    widget.setLabelText(QStringLiteral("Search"));
    widget.setText(QStringLiteral("dashboard"));
    widget.setLeadingIcon(widget.style()->standardIcon(QStyle::SP_FileDialogContentsView));
    widget.setTrailingIcon(widget.style()->standardIcon(QStyle::SP_ArrowForward));

    verifyOrUpdateBaseline(
        captureWidget(widget),
        QStringLiteral("filledtextfield_icons_light.png"));
}

void tst_FilledTextFieldRender::rendersPasswordToggleState()
{
    QtMaterial::QtMaterialFilledTextField widget;
    widget.setLabelText(QStringLiteral("Password"));
    widget.setEchoMode(QLineEdit::Password);
    widget.setText(QStringLiteral("secret123"));
    widget.setSupportingText(QStringLiteral("Use a strong password"));

    verifyOrUpdateBaseline(
        captureWidget(widget),
        QStringLiteral("filledtextfield_password_light.png"));
}

QTEST_MAIN(tst_FilledTextFieldRender)
#include "tst_filledtextfield_render.moc"