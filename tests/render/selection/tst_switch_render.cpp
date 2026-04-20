#include <QtTest/QtTest>
#include <QImage>
#include <QPainter>

#include "qtmaterial/widgets/selection/qtmaterialswitch.h"

class tst_SwitchRender : public QObject
{
    Q_OBJECT

private slots:
    void renderDefault();
    void renderChecked();
};

static QImage renderWidget(QWidget& widget)
{
    widget.resize(120, 48);
    QImage image(widget.size() * widget.devicePixelRatioF(), QImage::Format_ARGB32_Premultiplied);
    image.setDevicePixelRatio(widget.devicePixelRatioF());
    image.fill(Qt::transparent);
    widget.render(&image);
    return image;
}

void tst_SwitchRender::renderDefault()
{
    QtMaterial::QtMaterialSwitch widget;
    const QImage image = renderWidget(widget);
    QVERIFY(!image.isNull());
}

void tst_SwitchRender::renderChecked()
{
    QtMaterial::QtMaterialSwitch widget;
    widget.setChecked(true);
    const QImage image = renderWidget(widget);
    QVERIFY(!image.isNull());
}

QTEST_MAIN(tst_SwitchRender)
#include "tst_switch_render.moc"
