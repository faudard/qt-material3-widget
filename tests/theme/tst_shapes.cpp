#include <QtTest/QtTest>
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

class tst_Shapes : public QObject
{
    Q_OBJECT
private slots:
    void hasExpectedOrdering();
};

void tst_Shapes::hasExpectedOrdering() {
    QtMaterial::ThemeBuilder builder;
    const QtMaterial::Theme theme = builder.buildLightFromSeed(QColor("#6750A4"));
    QVERIFY(theme.shapes().radius(QtMaterial::ShapeRole::ExtraSmall) < theme.shapes().radius(QtMaterial::ShapeRole::Small));
    QVERIFY(theme.shapes().radius(QtMaterial::ShapeRole::Small) < theme.shapes().radius(QtMaterial::ShapeRole::Medium));
    QVERIFY(theme.shapes().radius(QtMaterial::ShapeRole::Medium) < theme.shapes().radius(QtMaterial::ShapeRole::Large));
}

QTEST_MAIN(tst_Shapes)
#include "tst_shapes.moc"
