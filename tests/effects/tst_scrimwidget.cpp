#include <QColor>
#include <QTest>

#include "qtmaterial/effects/qtmaterialscrimwidget.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

class TestableScrimWidget : public QtMaterial::QtMaterialScrimWidget
{
public:
    using QtMaterial::QtMaterialScrimWidget::QtMaterialScrimWidget;
    using QtMaterial::QtMaterialWidget::theme;
};

class tst_QtMaterialScrimWidget : public QObject
{
    Q_OBJECT

private slots:
    void defaultUsesThemeScrimColor();
    void explicitColorOverrideWins();
    void clearExplicitColorRestoresThemeFallback();
};

void tst_QtMaterialScrimWidget::defaultUsesThemeScrimColor()
{
    TestableScrimWidget scrim;

    const QColor expected =
        scrim.theme().colorScheme().color(QtMaterial::ColorRole::Scrim);

    QCOMPARE(scrim.scrimColor(), expected);
}

void tst_QtMaterialScrimWidget::explicitColorOverrideWins()
{
    TestableScrimWidget scrim;

    const QColor custom(10, 20, 30, 140);
    scrim.setScrimColor(custom);

    QCOMPARE(scrim.scrimColor(), custom);
}

void tst_QtMaterialScrimWidget::clearExplicitColorRestoresThemeFallback()
{
    TestableScrimWidget scrim;

    const QColor custom(90, 40, 20, 100);
    scrim.setScrimColor(custom);
    QCOMPARE(scrim.scrimColor(), custom);

    scrim.clearExplicitScrimColor();

    const QColor expected =
        scrim.theme().colorScheme().color(QtMaterial::ColorRole::Scrim);

    QCOMPARE(scrim.scrimColor(), expected);
}

QTEST_MAIN(tst_QtMaterialScrimWidget)
#include "tst_scrimwidget.moc"