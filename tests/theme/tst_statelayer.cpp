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
    QVERIFY(qFuzzyCompare(theme.stateLayer().hoverOpacity, qreal(0.08)));
    QVERIFY(qFuzzyCompare(theme.stateLayer().focusOpacity, qreal(0.10)));
    QVERIFY(qFuzzyCompare(theme.stateLayer().pressOpacity, qreal(0.10)));
    QVERIFY(qFuzzyCompare(theme.stateLayer().dragOpacity, qreal(0.16)));
}

QTEST_MAIN(tst_StateLayer)
#include "tst_statelayer.moc"
