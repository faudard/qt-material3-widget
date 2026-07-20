#include <QtTest>

#include "qtmaterial/core/qtmaterialwidget.h"
#include "qtmaterial/specs/qtmaterialmenuspecresolver.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"
#include "qtmaterial/widgets/navigation/qtmaterialmenu.h"

using namespace QtMaterial;

class tst_ThemedMenu : public QObject
{
    Q_OBJECT

private slots:
    void resolvesSpecFromParentContext();
    void followsThemeChanges();
    void explicitSpecRemainsPinned();
    void shortcutTextParticipatesInLayoutAndAccessibility();
    void keyboardTypeAheadSkipsDisabledAndSeparators();
    void activationTogglesCheckableItemOnce();
};

void tst_ThemedMenu::resolvesSpecFromParentContext()
{
    const Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));
    ThemeContext context(theme);

    QtMaterialWidget parent;
    parent.setThemeContext(&context);
    QtMaterialMenu menu(&parent);

    const MenuSpec expected =
        MenuSpecResolver().menuSpec(theme);

    QCOMPARE(menu.effectiveThemeContext(), &context);
    QVERIFY(!menu.hasExplicitSpec());
    QCOMPARE(
        menu.resolvedSpec().containerColor,
        expected.containerColor);
    QCOMPARE(
        menu.resolvedSpec().labelFont,
        expected.labelFont);
}

void tst_ThemedMenu::followsThemeChanges()
{
    Theme first = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));
    Theme second = ThemeBuilder().buildDarkFromSeed(
        QColor(QStringLiteral("#006874")));
    ThemeContext context(first);

    QtMaterialMenu menu;
    menu.setThemeContext(&context);
    const QColor before =
        menu.resolvedSpec().containerColor;

    QVERIFY(context.setTheme(second));

    const MenuSpec expected =
        MenuSpecResolver().menuSpec(second);
    QCOMPARE(
        menu.resolvedSpec().containerColor,
        expected.containerColor);
    QVERIFY(
        menu.resolvedSpec().containerColor != before);
}

void tst_ThemedMenu::explicitSpecRemainsPinned()
{
    ThemeContext context(
        ThemeBuilder().buildLightFromSeed(
            QColor(QStringLiteral("#6750A4"))));

    QtMaterialMenu menu;
    menu.setThemeContext(&context);

    MenuSpec explicitSpec = menu.spec();
    explicitSpec.containerColor =
        QColor(QStringLiteral("#123456"));
    explicitSpec.cornerRadius = 19.0;
    menu.setSpec(explicitSpec);

    QVERIFY(menu.hasExplicitSpec());

    QVERIFY(context.setTheme(
        ThemeBuilder().buildDarkFromSeed(
            QColor(QStringLiteral("#006874")))));

    QCOMPARE(
        menu.resolvedSpec().containerColor,
        QColor(QStringLiteral("#123456")));
    QCOMPARE(menu.resolvedSpec().cornerRadius, 19.0);

    menu.resetSpec();
    QVERIFY(!menu.hasExplicitSpec());
    QVERIFY(
        menu.resolvedSpec().containerColor
        != QColor(QStringLiteral("#123456")));
}

void tst_ThemedMenu::
shortcutTextParticipatesInLayoutAndAccessibility()
{
    QtMaterialMenu menu;
    const int open = menu.addItem(QStringLiteral("Open"));
    const QSize before = menu.sizeHint();

    menu.setItemShortcutText(
        open,
        QStringLiteral("Ctrl+O"));

    QVERIFY(menu.sizeHint().width() > before.width());
    QCOMPARE(
        menu.itemShortcutText(open),
        QStringLiteral("Ctrl+O"));
    QVERIFY(
        menu.itemAccessibleText(open).contains(
            QStringLiteral("Ctrl+O")));
}

void tst_ThemedMenu::
keyboardTypeAheadSkipsDisabledAndSeparators()
{
    QtMaterialMenu menu;
    menu.addItem(QStringLiteral("Alpha"));
    const int beta = menu.addItem(QStringLiteral("Beta"));
    menu.setItemEnabled(beta, false);
    menu.addSeparator();
    const int bravo = menu.addItem(QStringLiteral("Bravo"));
    menu.addItem(QStringLiteral("Charlie"));

    menu.setCurrentIndex(0);
    QTest::keyClick(&menu, Qt::Key_B);

    QCOMPARE(menu.currentIndex(), bravo);
}

void tst_ThemedMenu::
activationTogglesCheckableItemOnce()
{
    QtMaterialMenu menu;
    const int index =
        menu.addItem(QStringLiteral("Show grid"));
    menu.setItemCheckable(index, true);
    menu.setCurrentIndex(index);

    QSignalSpy activated(
        &menu,
        &QtMaterialMenu::activated);
    QTest::keyClick(&menu, Qt::Key_Return);

    QCOMPARE(activated.count(), 1);
    QVERIFY(menu.isItemChecked(index));
}

QTEST_MAIN(tst_ThemedMenu)
#include "tst_themedmenu.moc"
