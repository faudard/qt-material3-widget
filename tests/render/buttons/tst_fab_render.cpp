#include <QtTest/QtTest>
#include <QBuffer>
#include <QCryptographicHash>
#include <QDir>
#include <QFileInfo>
#include <QImage>
#include <QPixmap>

#include "qtmaterial/widgets/buttons/qtmaterialfab.h"

class tst_FabRender : public QObject
{
    Q_OBJECT
private slots:
    void rendersDefaultState();
    void rendersDisabledState();

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
            QStringLiteral("../../../tools/render-baselines/buttons/") + name);
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

void tst_FabRender::rendersDefaultState()
{
    QtMaterial::QtMaterialFab widget;
    widget.resize(widget.sizeHint());
    widget.show();
    QVERIFY(QTest::qWaitForWindowExposed(&widget));

    const QImage image = widget.grab().toImage();
    QVERIFY(!image.isNull());
    verifyOrUpdateBaseline(image, QStringLiteral("fab_default_light.png"));
}

void tst_FabRender::rendersDisabledState()
{
    QtMaterial::QtMaterialFab widget;
    widget.setEnabled(false);
    widget.resize(widget.sizeHint());
    widget.show();
    QVERIFY(QTest::qWaitForWindowExposed(&widget));

    const QImage image = widget.grab().toImage();
    QVERIFY(!image.isNull());
    verifyOrUpdateBaseline(image, QStringLiteral("fab_disabled_light.png"));
}

QTEST_MAIN(tst_FabRender)
#include "tst_fab_render.moc"
