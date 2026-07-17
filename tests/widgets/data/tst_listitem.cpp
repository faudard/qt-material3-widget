#include <QtTest>

#include <QImage>
#include <QPainter>
#include <QSignalSpy>

#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"
#include "qtmaterial/widgets/data/qtmateriallistitem.h"

using namespace QtMaterial;

class tst_ListItem : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void construction();
    void selectionApi();
    void keyboardActivation();
    void sizeHintTouchTarget();
    void usesResolvedThemeSpec();
    void refreshesAfterThemeChange();
};

void tst_ListItem::construction()
{
    QtMaterialListItem item;

    QCOMPARE(
        item.headlineText(),
        QString());
    QCOMPARE(
        item.supportingText(),
        QString());
    QCOMPARE(
        item.densityVariant(),
        QtMaterialListItem::DensityVariant::Standard);
    QVERIFY(item.focusPolicy() == Qt::StrongFocus);
    QVERIFY(item.resolvedSpec().headlineColor.isValid());
}

void tst_ListItem::selectionApi()
{
    QtMaterialListItem item;
    QSignalSpy spy(
        &item,
        &QtMaterialListItem::selectionChanged);

    item.setSelected(true);

    QVERIFY(item.isSelected());
    QCOMPARE(spy.count(), 1);

    item.setSelected(true);
    QCOMPARE(spy.count(), 1);
}

void tst_ListItem::keyboardActivation()
{
    QtMaterialListItem item;
    QSignalSpy clickedSpy(
        &item,
        &QtMaterialListItem::clicked);
    QSignalSpy activatedSpy(
        &item,
        &QtMaterialListItem::activated);

    item.setFocus();
    QTest::keyClick(
        &item,
        Qt::Key_Return);

    QCOMPARE(clickedSpy.count(), 1);
    QCOMPARE(activatedSpy.count(), 1);
}

void tst_ListItem::sizeHintTouchTarget()
{
    QtMaterialListItem item;

    item.setDensityVariant(
        QtMaterialListItem::DensityVariant::Compact);
    QCOMPARE(
        item.sizeHint().height(),
        item.resolvedSpec().compactMinHeight);

    item.setDensityVariant(
        QtMaterialListItem::DensityVariant::Standard);
    QCOMPARE(
        item.sizeHint().height(),
        item.resolvedSpec().minHeight);

    item.setDensityVariant(
        QtMaterialListItem::DensityVariant::Large);
    QCOMPARE(
        item.sizeHint().height(),
        item.resolvedSpec().largeMinHeight);
}

void tst_ListItem::usesResolvedThemeSpec()
{
    Theme theme =
        ThemeBuilder().buildLightFromSeed(
            QColor(QStringLiteral("#6750A4")));
    theme.colorScheme().setColor(
        ColorRole::OnSurface,
        QColor(Qt::red));

    ThemeContext context(theme);
    QtMaterialListItem item;
    item.setThemeContext(&context);

    QCOMPARE(
        item.resolvedSpec().headlineColor,
        QColor(Qt::red));
}

void tst_ListItem::refreshesAfterThemeChange()
{
    Theme first =
        ThemeBuilder().buildLightFromSeed(
            QColor(QStringLiteral("#6750A4")));
    first.colorScheme().setColor(
        ColorRole::OnSurface,
        QColor(Qt::red));

    Theme second = first;
    second.colorScheme().setColor(
        ColorRole::OnSurface,
        QColor(Qt::blue));

    ThemeContext context(first);
    QtMaterialListItem item;
    item.setThemeContext(&context);

    QCOMPARE(
        item.resolvedSpec().headlineColor,
        QColor(Qt::red));

    QVERIFY(context.setTheme(second));

    QCOMPARE(
        item.resolvedSpec().headlineColor,
        QColor(Qt::blue));
}

QTEST_MAIN(tst_ListItem)

#include "tst_listitem.moc"
