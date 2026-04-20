#include <QtTest/QtTest>
#include <QBuffer>
#include <QCryptographicHash>
#include <QDir>
#include <QFileInfo>
#include <QImage>
#include <qlineedit.h>

#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"

class tst_OutlinedTextFieldRender : public QObject
{
    Q_OBJECT
private slots:
    void rendersDefaultState();
    void rendersFocusedState();
    void rendersErrorState();

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
};

void tst_OutlinedTextFieldRender::rendersDefaultState()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
    widget.setLabelText(QStringLiteral("Email"));
    widget.setSupportingText(QStringLiteral("We will not spam you"));
    widget.resize(widget.sizeHint());
    widget.show();
    QVERIFY(QTest::qWaitForWindowExposed(&widget));

    const QImage image = widget.grab().toImage();
    QVERIFY(!image.isNull());
    verifyOrUpdateBaseline(image, QStringLiteral("outlinedtextfield_default_light.png"));
}

void tst_OutlinedTextFieldRender::rendersFocusedState()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
    widget.setLabelText(QStringLiteral("Email"));
    widget.setSupportingText(QStringLiteral("We will not spam you"));
    widget.resize(widget.sizeHint());
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
    widget.resize(widget.sizeHint());
    widget.show();
    QVERIFY(QTest::qWaitForWindowExposed(&widget));

    const QImage image = widget.grab().toImage();
    QVERIFY(!image.isNull());
    verifyOrUpdateBaseline(image, QStringLiteral("outlinedtextfield_error_light.png"));
}

QTEST_MAIN(tst_OutlinedTextFieldRender)
#include "tst_outlinedtextfield_render.moc"
