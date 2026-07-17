#include <QtTest>

#include <QPalette>

#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"
#include "qtmaterial/widgets/data/qtmaterialgridlist.h"
#include "qtmaterial/widgets/data/qtmateriallist.h"
#include "qtmaterial/widgets/data/qtmateriallistitem.h"

using namespace QtMaterial;

class tst_DataListGridResolved
    : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void listOwnsItems();
    void listSynchronizesDividers();
    void listRefreshesAfterThemeChange();
    void gridListUsesResolvedSpec();
    void gridListLocalMetricsWin();
    void gridListRefreshesAfterThemeChange();
};

void tst_DataListGridResolved::listOwnsItems()
{
    QtMaterialList list;
    auto* first =
        new QtMaterialListItem;
    auto* second =
        new QtMaterialListItem;

    list.addItem(first);
    list.addItem(second);

    QCOMPARE(list.count(), 2);
    QCOMPARE(list.itemAt(0), first);
    QCOMPARE(list.itemAt(1), second);
    QCOMPARE(first->parentWidget(), &list);
    QCOMPARE(second->parentWidget(), &list);

    list.removeItem(first);

    QCOMPARE(list.count(), 1);
    QCOMPARE(first->parentWidget(), nullptr);

    delete first;
}

void tst_DataListGridResolved::
listSynchronizesDividers()
{
    QtMaterialList list;
    auto* first =
        new QtMaterialListItem;
    auto* second =
        new QtMaterialListItem;

    list.addItem(first);
    list.addItem(second);
    list.setDividersVisible(true);

    QVERIFY(first->isDividerVisible());
    QVERIFY(!second->isDividerVisible());

    list.setDividersVisible(false);

    QVERIFY(!first->isDividerVisible());
    QVERIFY(!second->isDividerVisible());
}

void tst_DataListGridResolved::
listRefreshesAfterThemeChange()
{
    Theme first =
        ThemeBuilder().buildLightFromSeed(
            QColor(
                QStringLiteral("#6750A4")));
    first.colorScheme().setColor(
        ColorRole::Surface,
        QColor(Qt::red));

    Theme second = first;
    second.colorScheme().setColor(
        ColorRole::Surface,
        QColor(Qt::blue));

    ThemeContext context(first);
    QtMaterialList list;
    list.setThemeContext(&context);

    QCOMPARE(
        list.resolvedSpec().containerColor,
        QColor(Qt::red));

    QVERIFY(context.setTheme(second));

    QCOMPARE(
        list.resolvedSpec().containerColor,
        QColor(Qt::blue));
}

void tst_DataListGridResolved::
gridListUsesResolvedSpec()
{
    Theme theme =
        ThemeBuilder().buildLightFromSeed(
            QColor(
                QStringLiteral("#006874")));
    theme.colorScheme().setColor(
        ColorRole::SecondaryContainer,
        QColor(Qt::green));

    ThemeContext context(theme);
    QtMaterialGridList grid;
    grid.setThemeContext(&context);

    QCOMPARE(
        grid.resolvedSpec().itemSelectedColor,
        QColor(Qt::green));
    QCOMPARE(
        grid.palette().color(
            QPalette::Highlight),
        QColor(Qt::green));
}

void tst_DataListGridResolved::
gridListLocalMetricsWin()
{
    QtMaterialGridList grid;

    grid.setColumns(5);
    grid.setCellExtent(
        QSize(210, 170));

    QCOMPARE(grid.columns(), 5);
    QCOMPARE(
        grid.cellExtent(),
        QSize(210, 170));

    grid.addGridItem(
        QStringLiteral("Alpha"),
        QStringLiteral("Supporting"));

    QCOMPARE(
        grid.item(0)->sizeHint().height(),
        170);
}

void tst_DataListGridResolved::
gridListRefreshesAfterThemeChange()
{
    Theme first =
        ThemeBuilder().buildLightFromSeed(
            QColor(
                QStringLiteral("#6750A4")));
    first.colorScheme().setColor(
        ColorRole::OnSurface,
        QColor(Qt::red));

    Theme second = first;
    second.colorScheme().setColor(
        ColorRole::OnSurface,
        QColor(Qt::blue));

    ThemeContext context(first);
    QtMaterialGridList grid;
    grid.setThemeContext(&context);

    QCOMPARE(
        grid.resolvedSpec().foregroundColor,
        QColor(Qt::red));

    QVERIFY(context.setTheme(second));

    QCOMPARE(
        grid.resolvedSpec().foregroundColor,
        QColor(Qt::blue));
}

QTEST_MAIN(tst_DataListGridResolved)

#include "tst_data_list_grid_resolved.moc"
