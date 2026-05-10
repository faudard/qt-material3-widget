#include <QtTest/QtTest>

#include <QAccessible>
#include <QIcon>
#include <QPainter>
#include <QPixmap>

#include <memory>

#include "qtmaterial/widgets/buttons/qtmaterialextendedfab.h"
#include "qtmaterial/widgets/buttons/qtmaterialfab.h"
#include "qtmaterial/widgets/buttons/qtmaterialiconbutton.h"

namespace {

QIcon contractIcon()
{
    QPixmap pixmap(24, 24);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.drawEllipse(QRectF(4, 4, 16, 16));
    painter.end();

    return QIcon(pixmap);
}

} // namespace

class tst_IconFabAccessibility : public QObject
{
    Q_OBJECT

private slots:
    void iconButtonRequiresExplicitAccessibleName();
    void iconButtonDoesNotTreatTooltipAsStrictAccessibleName();
    void iconButtonCanUseFallbacksWhenRequirementIsDisabled();
    void fabRequiresExplicitAccessibleName();
    void fabUsesIconAccessibleName();
    void fabPrefersAccessibleNameOverIconAccessibleName();
    void fabDoesNotTreatTooltipAsStrictAccessibleName();
    void extendedFabUsesVisibleTextAsAccessibleName();
    void accessibleInterfacesExposeButtonRole();
};

void tst_IconFabAccessibility::iconButtonRequiresExplicitAccessibleName()
{
    QtMaterial::QtMaterialIconButton button(contractIcon());

    QVERIFY(button.requiresAccessibleName());
    QVERIFY(!button.hasUsableAccessibleName());
    QVERIFY(button.effectiveAccessibleName().isEmpty());

    button.setAccessibleName(QStringLiteral("Search"));

    QVERIFY(button.hasUsableAccessibleName());
    QCOMPARE(button.effectiveAccessibleName(), QStringLiteral("Search"));
}

void tst_IconFabAccessibility::iconButtonDoesNotTreatTooltipAsStrictAccessibleName()
{
    QtMaterial::QtMaterialIconButton button(contractIcon());
    button.setToolTip(QStringLiteral("Search"));

    QVERIFY(button.requiresAccessibleName());
    QVERIFY(!button.hasUsableAccessibleName());
    QVERIFY(button.effectiveAccessibleName().isEmpty());
}

void tst_IconFabAccessibility::iconButtonCanUseFallbacksWhenRequirementIsDisabled()
{
    QtMaterial::QtMaterialIconButton button(contractIcon());
    button.setRequiresAccessibleName(false);
    button.setToolTip(QStringLiteral("Search"));

    QVERIFY(!button.requiresAccessibleName());
    QVERIFY(button.hasUsableAccessibleName());
    QCOMPARE(button.effectiveAccessibleName(), QStringLiteral("Search"));
}

void tst_IconFabAccessibility::fabRequiresExplicitAccessibleName()
{
    QtMaterial::QtMaterialFab button(contractIcon());

    QVERIFY(button.requiresAccessibleName());
    QVERIFY(!button.hasUsableAccessibleName());
    QVERIFY(button.effectiveAccessibleName().isEmpty());

    button.setAccessibleName(QStringLiteral("Create"));

    QVERIFY(button.hasUsableAccessibleName());
    QCOMPARE(button.effectiveAccessibleName(), QStringLiteral("Create"));
}

void tst_IconFabAccessibility::fabUsesIconAccessibleName()
{
    QtMaterial::QtMaterialFab button(contractIcon());
    button.setIconAccessibleName(QStringLiteral("Create item"));

    QVERIFY(button.requiresAccessibleName());
    QVERIFY(button.hasUsableAccessibleName());
    QCOMPARE(button.effectiveAccessibleName(), QStringLiteral("Create item"));
}

void tst_IconFabAccessibility::fabPrefersAccessibleNameOverIconAccessibleName()
{
    QtMaterial::QtMaterialFab button(contractIcon());
    button.setIconAccessibleName(QStringLiteral("Create item"));
    button.setAccessibleName(QStringLiteral("Create project"));

    QVERIFY(button.hasUsableAccessibleName());
    QCOMPARE(button.effectiveAccessibleName(), QStringLiteral("Create project"));
}

void tst_IconFabAccessibility::fabDoesNotTreatTooltipAsStrictAccessibleName()
{
    QtMaterial::QtMaterialFab button(contractIcon());
    button.setToolTip(QStringLiteral("Create"));

    QVERIFY(button.requiresAccessibleName());
    QVERIFY(!button.hasUsableAccessibleName());
    QVERIFY(button.effectiveAccessibleName().isEmpty());
}

void tst_IconFabAccessibility::extendedFabUsesVisibleTextAsAccessibleName()
{
    QtMaterial::QtMaterialExtendedFab button(contractIcon(), QStringLiteral("Create"));

    QVERIFY(button.hasUsableAccessibleName());
    QCOMPARE(button.effectiveAccessibleName(), QStringLiteral("Create"));

    button.setAccessibleName(QStringLiteral("Create project"));
    QVERIFY(button.hasUsableAccessibleName());
    QCOMPARE(button.effectiveAccessibleName(), QStringLiteral("Create project"));
}

void tst_IconFabAccessibility::accessibleInterfacesExposeButtonRole()
{
#if QT_CONFIG(accessibility)
    QtMaterial::QtMaterialIconButton iconButton(contractIcon());
    iconButton.setAccessibleName(QStringLiteral("Search"));

    QtMaterial::QtMaterialFab fab(contractIcon());
    fab.setIconAccessibleName(QStringLiteral("Create"));

    QtMaterial::QtMaterialExtendedFab extendedFab(contractIcon(), QStringLiteral("Create"));

    std::unique_ptr<QAccessibleInterface> iconIface(QAccessible::queryAccessibleInterface(&iconButton));
    std::unique_ptr<QAccessibleInterface> fabIface(QAccessible::queryAccessibleInterface(&fab));
    std::unique_ptr<QAccessibleInterface> extendedIface(QAccessible::queryAccessibleInterface(&extendedFab));

    QVERIFY(iconIface != nullptr);
    QVERIFY(fabIface != nullptr);
    QVERIFY(extendedIface != nullptr);

    QCOMPARE(iconIface->role(), QAccessible::Button);
    QCOMPARE(fabIface->role(), QAccessible::Button);
    QCOMPARE(extendedIface->role(), QAccessible::Button);
#else
    QSKIP("Qt accessibility is not available in this build");
#endif
}

QTEST_MAIN(tst_IconFabAccessibility)
#include "tst_icon_fab_accessibility.moc"
