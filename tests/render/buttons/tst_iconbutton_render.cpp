#include <QtTest/QtTest>
#include <QImage>
#include <QPainter>

#include "qtmaterial/widgets/buttons/qtmaterialiconbutton.h"

class tst_QtMaterialIconButtonRender : public QObject
{
    Q_OBJECT
private slots:
    void rendersDefaultState();
};

void tst_QtMaterialIconButtonRender::rendersDefaultState()
{
    QtMaterial::QtMaterialIconButton button;
    button.resize(48, 48);
    button.setIcon(QIcon::fromTheme(QStringLiteral("document-open")));

    QImage image(button.size(), QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);

    button.render(&image);

    QVERIFY(!image.isNull());
    QCOMPARE(image.size(), QSize(48, 48));
}

QTEST_MAIN(tst_QtMaterialIconButtonRender)
#include "tst_iconbutton_render.moc"
