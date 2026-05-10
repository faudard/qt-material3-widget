#include <QtTest/QtTest>

#include <QColor>
#include <QIcon>
#include <QPainter>
#include <QPixmap>

#include "widgets/buttons/private/qtmaterialbuttonrenderhelper_p.h"
#include "qtmaterial/specs/qtmaterialbuttonspec.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"

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

QtMaterial::ButtonSpec iconSpec()
{
    QtMaterial::ButtonSpec spec;
    spec.iconSize = 24;
    spec.touchTarget = QSize(40, 40);
    spec.containerHeight = 40;
    spec.horizontalPadding = 8;
    spec.iconSpacing = 8;
    return spec;
}

} // namespace

class tst_ButtonIconHiDpi : public QObject
{
    Q_OBJECT

private slots:
    void tintedIconPreservesBaseDevicePixelRatio_data();
    void tintedIconPreservesBaseDevicePixelRatio();
    void tintedIconCacheReturnsEquivalentPixmap();
    void nullIconReturnsNullPixmap();
};

void tst_ButtonIconHiDpi::tintedIconPreservesBaseDevicePixelRatio_data()
{
    QTest::addColumn<qreal>("dpr");

    QTest::newRow("1.0") << 1.0;
    QTest::newRow("1.5") << 1.5;
    QTest::newRow("2.0") << 2.0;
}

void tst_ButtonIconHiDpi::tintedIconPreservesBaseDevicePixelRatio()
{
    QFETCH(qreal, dpr);

    QtMaterial::QtMaterialFilledButton button;
    button.setIcon(hidpiIcon(dpr));

    const QtMaterial::ButtonSpec spec = iconSpec();
    const QPixmap base = button.icon().pixmap(QSize(spec.iconSize, spec.iconSize), QIcon::Normal, QIcon::Off);
    const QPixmap tinted = QtMaterial::ButtonRenderHelper::tintedIconPixmap(&button, spec, QColor(Qt::red));

    QVERIFY(!base.isNull());
    QVERIFY(!tinted.isNull());
    QCOMPARE(tinted.size(), base.size());
    QCOMPARE(tinted.devicePixelRatio(), base.devicePixelRatio());
}

void tst_ButtonIconHiDpi::tintedIconCacheReturnsEquivalentPixmap()
{
    QtMaterial::QtMaterialFilledButton button;
    button.setIcon(hidpiIcon(2.0));

    const QtMaterial::ButtonSpec spec = iconSpec();
    const QPixmap first = QtMaterial::ButtonRenderHelper::tintedIconPixmap(&button, spec, QColor(Qt::blue));
    const QPixmap second = QtMaterial::ButtonRenderHelper::tintedIconPixmap(&button, spec, QColor(Qt::blue));

    QVERIFY(!first.isNull());
    QVERIFY(!second.isNull());
    QCOMPARE(first.size(), second.size());
    QCOMPARE(first.devicePixelRatio(), second.devicePixelRatio());
}

void tst_ButtonIconHiDpi::nullIconReturnsNullPixmap()
{
    QtMaterial::QtMaterialFilledButton button;
    const QPixmap tinted = QtMaterial::ButtonRenderHelper::tintedIconPixmap(&button, iconSpec(), QColor(Qt::red));
    QVERIFY(tinted.isNull());
}

QTEST_MAIN(tst_ButtonIconHiDpi)
#include "tst_button_icon_hidpi.moc"
