#include <QtTest/QtTest>
#include <QImage>
#include <QPainter>

#include "qtmaterial/widgets/data/qtmaterialdivider.h"

using namespace QtMaterial;

class tst_Divider : public QObject
{
    Q_OBJECT

private slots:
    void defaultConstruction();
    void thicknessApi();
    void colorApi();
    void factories();
    void renderHorizontalInsets();
    void renderVerticalInsets();
};

static QImage renderWidget(QWidget &widget, const QSize &size)
{
    widget.resize(size);

    QImage image(size, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    widget.render(&painter);

    return image;
}

void tst_Divider::defaultConstruction()
{
    QtMaterialDivider divider;

    QCOMPARE(divider.orientation(), Qt::Horizontal);
    QCOMPARE(divider.leadingInset(), 0);
    QCOMPARE(divider.trailingInset(), 0);
    QCOMPARE(divider.thickness(), 1);
    QVERIFY(!divider.color().isValid());
}

void tst_Divider::thicknessApi()
{
    QtMaterialDivider divider;

    divider.setThickness(4);
    QCOMPARE(divider.thickness(), 4);
    QCOMPARE(divider.sizeHint().height(), 4);

    divider.setThickness(0);
    QCOMPARE(divider.thickness(), 1);

    divider.setOrientation(Qt::Vertical);
    divider.setThickness(3);
    QCOMPARE(divider.sizeHint().width(), 3);
}

void tst_Divider::colorApi()
{
    QtMaterialDivider divider;

    const QColor red(255, 0, 0);
    divider.setColor(red);
    QCOMPARE(divider.color(), red);

    divider.resetColor();
    QVERIFY(!divider.color().isValid());
}

void tst_Divider::factories()
{
    auto *horizontal = QtMaterialDivider::horizontal();
    QVERIFY(horizontal != nullptr);
    QCOMPARE(horizontal->orientation(), Qt::Horizontal);
    delete horizontal;

    auto *vertical = QtMaterialDivider::vertical();
    QVERIFY(vertical != nullptr);
    QCOMPARE(vertical->orientation(), Qt::Vertical);
    delete vertical;
}

void tst_Divider::renderHorizontalInsets()
{
    QtMaterialDivider divider(Qt::Horizontal);
    divider.setColor(Qt::black);
    divider.setThickness(2);
    divider.setLeadingInset(4);
    divider.setTrailingInset(6);

    const QImage image = renderWidget(divider, QSize(20, 6));

    const int y0 = 2;
    const int y1 = 3;

    for (int x = 0; x < 4; ++x) {
        QVERIFY(image.pixelColor(x, y0).alpha() == 0);
        QVERIFY(image.pixelColor(x, y1).alpha() == 0);
    }

    for (int x = 4; x < 14; ++x) {
        QCOMPARE(image.pixelColor(x, y0), QColor(Qt::black));
        QCOMPARE(image.pixelColor(x, y1), QColor(Qt::black));
    }

    for (int x = 14; x < 20; ++x) {
        QVERIFY(image.pixelColor(x, y0).alpha() == 0);
        QVERIFY(image.pixelColor(x, y1).alpha() == 0);
    }
}

void tst_Divider::renderVerticalInsets()
{
    QtMaterialDivider divider(Qt::Vertical);
    divider.setColor(Qt::black);
    divider.setThickness(2);
    divider.setLeadingInset(3);
    divider.setTrailingInset(5);

    const QImage image = renderWidget(divider, QSize(6, 20));

    const int x0 = 2;
    const int x1 = 3;

    for (int y = 0; y < 3; ++y) {
        QVERIFY(image.pixelColor(x0, y).alpha() == 0);
        QVERIFY(image.pixelColor(x1, y).alpha() == 0);
    }

    for (int y = 3; y < 15; ++y) {
        QCOMPARE(image.pixelColor(x0, y), QColor(Qt::black));
        QCOMPARE(image.pixelColor(x1, y), QColor(Qt::black));
    }

    for (int y = 15; y < 20; ++y) {
        QVERIFY(image.pixelColor(x0, y).alpha() == 0);
        QVERIFY(image.pixelColor(x1, y).alpha() == 0);
    }
}

QTEST_MAIN(tst_Divider)
#include "tst_divider.moc"
