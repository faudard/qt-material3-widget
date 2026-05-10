#include <QtTest/QtTest>

#include <QAbstractButton>
#include <QIcon>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <memory>

#include "qtmaterial/widgets/buttons/qtmaterialextendedfab.h"
#include "qtmaterial/widgets/buttons/qtmaterialfab.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialiconbutton.h"

namespace {

QIcon hidpiIcon(qreal dpr)
{
    QPixmap pixmap(qRound(24 * dpr), qRound(24 * dpr));
    pixmap.setDevicePixelRatio(dpr);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.drawEllipse(QRectF(4, 4, 16, 16));
    painter.end();

    QIcon icon;
    icon.addPixmap(pixmap, QIcon::Normal, QIcon::Off);
    return icon;
}

struct ButtonCase
{
    QString name;
    std::function<std::unique_ptr<QAbstractButton>(qreal)> create;
};

QVector<ButtonCase> iconCases()
{
    return {
        {QStringLiteral("filled"), [] (qreal dpr) {
             auto button = std::make_unique<QtMaterial::QtMaterialFilledButton>();
             button->setText(QStringLiteral("Save"));
             button->setIcon(hidpiIcon(dpr));
             return button;
         }},
        {QStringLiteral("icon"), [] (qreal dpr) {
             auto button = std::make_unique<QtMaterial::QtMaterialIconButton>(hidpiIcon(dpr));
             button->setAccessibleName(QStringLiteral("Search"));
             return button;
         }},
        {QStringLiteral("fab"), [] (qreal dpr) {
             auto button = std::make_unique<QtMaterial::QtMaterialFab>(hidpiIcon(dpr));
             button->setAccessibleName(QStringLiteral("Create"));
             return button;
         }},
        {QStringLiteral("extended-fab"), [] (qreal dpr) {
             return std::make_unique<QtMaterial::QtMaterialExtendedFab>(hidpiIcon(dpr), QStringLiteral("Create"));
         }},
    };
}

QImage renderButton(QAbstractButton& button, qreal devicePixelRatio)
{
    const QSize logicalSize = button.sizeHint().expandedTo(QSize(144, 56));
    button.resize(logicalSize);
    button.show();
    QTest::qWaitForWindowExposed(&button);

    QPixmap pixmap(qCeil(logicalSize.width() * devicePixelRatio),
                   qCeil(logicalSize.height() * devicePixelRatio));
    pixmap.setDevicePixelRatio(devicePixelRatio);
    pixmap.fill(Qt::transparent);

    button.render(&pixmap);
    return pixmap.toImage();
}

bool hasPaintedPixels(const QImage& image)
{
    if (image.isNull()) {
        return false;
    }

    for (int y = 0; y < image.height(); ++y) {
        const QRgb* line = reinterpret_cast<const QRgb*>(image.constScanLine(y));
        for (int x = 0; x < image.width(); ++x) {
            if (qAlpha(line[x]) != 0) {
                return true;
            }
        }
    }

    return false;
}

} // namespace

class tst_ButtonIconHiDpi : public QObject
{
    Q_OBJECT

private slots:
    void rendersIconButtonsAtDevicePixelRatios_data();
    void rendersIconButtonsAtDevicePixelRatios();
    void changingIconSizeKeepsHiDpiRenderValid();
    void nullIconStillRendersButtonContainer();
};

void tst_ButtonIconHiDpi::rendersIconButtonsAtDevicePixelRatios_data()
{
    QTest::addColumn<qreal>("dpr");

    QTest::newRow("1.0") << 1.0;
    QTest::newRow("1.5") << 1.5;
    QTest::newRow("2.0") << 2.0;
}

void tst_ButtonIconHiDpi::rendersIconButtonsAtDevicePixelRatios()
{
    QFETCH(qreal, dpr);

    for (const ButtonCase& testCase : iconCases()) {
        auto button = testCase.create(dpr);
        const QImage image = renderButton(*button, dpr);

        QVERIFY2(!image.isNull(), qPrintable(testCase.name));
        QVERIFY2(hasPaintedPixels(image), qPrintable(testCase.name));
        QVERIFY2(image.width() >= qCeil(40 * dpr), qPrintable(testCase.name));
        QVERIFY2(image.height() >= qCeil(40 * dpr), qPrintable(testCase.name));
    }
}

void tst_ButtonIconHiDpi::changingIconSizeKeepsHiDpiRenderValid()
{
    for (const ButtonCase& testCase : iconCases()) {
        auto button = testCase.create(2.0);
        button->setIconSize(QSize(28, 28));
        const QImage image = renderButton(*button, 2.0);

        QVERIFY2(!image.isNull(), qPrintable(testCase.name));
        QVERIFY2(hasPaintedPixels(image), qPrintable(testCase.name));
    }
}

void tst_ButtonIconHiDpi::nullIconStillRendersButtonContainer()
{
    QtMaterial::QtMaterialFilledButton button;
    button.setText(QStringLiteral("Save"));
    const QImage image = renderButton(button, 2.0);

    QVERIFY(!image.isNull());
    QVERIFY(hasPaintedPixels(image));
}

QTEST_MAIN(tst_ButtonIconHiDpi)
#include "tst_button_icon_hidpi.moc"
