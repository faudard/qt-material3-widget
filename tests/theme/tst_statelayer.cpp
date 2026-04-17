#include <QtTest/QtTest>
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

class tst_StateLayer : public QObject
{
    Q_OBJECT
private slots:
    void hasDefaultOpacities();
};

void tst_StateLayer::hasDefaultOpacities() {
    QtMaterial::ThemeBuilder builder;
    const QtMaterial::Theme theme = builder.buildLightFromSeed(QColor("#6750A4"));
    QCOMPARE(theme.stateLayer().hoverOpacity(), 0.08);
    QCOMPARE(theme.stateLayer().focusOpacity(), 0.10);
    QCOMPARE(theme.stateLayer().pressOpacity(), 0.10);
    QCOMPARE(theme.stateLayer().dragOpacity(), 0.16);
}

QTEST_MAIN(tst_StateLayer)
#include "tst_statelayer.moc"
