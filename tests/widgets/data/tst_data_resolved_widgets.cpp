#include <QtTest>

#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"
#include "qtmaterial/widgets/data/qtmaterialdivider.h"
#include "qtmaterial/widgets/data/qtmateriallistitem.h"

using namespace QtMaterial;

class tst_DataResolvedWidgets : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void dividerUsesThemeColor();
    void dividerLocalColorWins();
    void widgetsExposeResolvedSpecs();
};

void tst_DataResolvedWidgets::dividerUsesThemeColor()
{
    Theme theme =
        ThemeBuilder().buildLightFromSeed(
            QColor(QStringLiteral("#006874")));
    theme.colorScheme().setColor(
        ColorRole::OutlineVariant,
        QColor(Qt::green));

    ThemeContext context(theme);
    QtMaterialDivider divider;
    divider.setThemeContext(&context);

    QCOMPARE(
        divider.resolvedSpec().color,
        QColor(Qt::green));
}

void tst_DataResolvedWidgets::dividerLocalColorWins()
{
    ThemeContext context(
        ThemeBuilder().buildLightFromSeed(
            QColor(QStringLiteral("#6750A4"))));

    QtMaterialDivider divider;
    divider.setThemeContext(&context);
    divider.setColor(QColor(Qt::black));

    QCOMPARE(
        divider.resolvedSpec().color,
        QColor(Qt::black));
    QCOMPARE(
        divider.resolvedSpec().insetColor,
        QColor(Qt::black));
}

void tst_DataResolvedWidgets::widgetsExposeResolvedSpecs()
{
    QtMaterialListItem item;
    QtMaterialDivider divider;

    QVERIFY(item.resolvedSpec().headlineColor.isValid());
    QVERIFY(divider.resolvedSpec().color.isValid());
    QVERIFY(item.resolvedSpec().minHeight > 0);
    QVERIFY(divider.resolvedSpec().thickness > 0);
}

QTEST_MAIN(tst_DataResolvedWidgets)

#include "tst_data_resolved_widgets.moc"
