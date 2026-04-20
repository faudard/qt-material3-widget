#include <QtTest/QtTest>
#include <QImage>
#include <QPainter>

#include "qtmaterial/widgets/surfaces/qtmaterialdialog.h"

class tst_DialogRender : public QObject
{
    Q_OBJECT

private slots:
    void rendersDefaultLight();
};

void tst_DialogRender::rendersDefaultLight()
{
    QWidget host;
    host.resize(800, 600);
    QtMaterialDialog dialog(&host);
    dialog.resize(host.size());
    dialog.open();

    QImage image(dialog.size(), QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    dialog.render(&painter);
    QVERIFY(!image.isNull());
}

QTEST_MAIN(tst_DialogRender)
#include "tst_dialog_render.moc"
