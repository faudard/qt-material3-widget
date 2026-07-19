#include <QtTest/QtTest>

#include <QFont>
#include <QRect>

#include "qtmaterial/theme/qtmaterialtheme.h"
#include "qtmaterial/specs/qtmaterialtextfieldspecresolver.h"
#include "qtmaterialtextfieldshellhelper_p.h"

using QtMaterial::QtMaterialTextFieldShellHelper;
using QtMaterial::TextFieldSpec;
using QtMaterial::Theme;

namespace {

struct Fixture {
    Theme theme;
    QFont font;

    TextFieldSpec outlinedSpec() const
    {
        return QtMaterial::TextFieldSpecResolver().outlinedTextFieldSpec(theme);
    }

    TextFieldSpec filledSpec() const
    {
        return QtMaterial::TextFieldSpecResolver().filledTextFieldSpec(theme);
    }

    static QRect boundsFor(const TextFieldSpec& spec, int width = 360)
    {
        return QRect(0,
                     0,
                     width,
                     spec.topLabelHeight + spec.minHeight + spec.supportingTopSpacing
                         + spec.supportingHeight);
    }
};

QtMaterialTextFieldShellHelper::Layout layoutFor(
    const TextFieldSpec& spec,
    const Theme& theme,
    QtMaterialTextFieldShellHelper::Variant variant,
    const QtMaterialTextFieldShellHelper::Accessories& accessories,
    const QFont& font,
    int width = 360)
{
    return QtMaterialTextFieldShellHelper::layoutFor(
        Fixture::boundsFor(spec, width), spec, variant, accessories, font);
}

} // namespace

class tst_TextFieldShellHelper : public QObject
{
    Q_OBJECT

private slots:
    void emptyAccessoriesKeepEditorInsideContainer();
    void prefixAndSuffixReserveEditorSpace();
    void leadingAndTrailingIconsReserveEditorSpace();
    void clearActionUsesEndActionRectAndSuppressesTrailingIcon();
    void passwordToggleUsesEndActionRect();
    void rtlMirrorsLeadingSideAccessories();
    void smallWidthDoesNotProduceNegativeRects();
    void filledVariantOffsetsEditorDown();
};

void tst_TextFieldShellHelper::emptyAccessoriesKeepEditorInsideContainer()
{
    const Fixture fixture;
    const TextFieldSpec spec = fixture.outlinedSpec();

    const auto layout = layoutFor(spec,
                                  fixture.theme,
                                  QtMaterialTextFieldShellHelper::Variant::Outlined,
                                  {},
                                  fixture.font);

    QVERIFY(!layout.containerRect.isEmpty());
    QVERIFY(!layout.editorRect.isEmpty());
    QVERIFY(layout.containerRect.contains(layout.editorRect));
    QVERIFY(layout.labelRect.width() > 0);
    QVERIFY(layout.supportingRect.width() > 0);
}

void tst_TextFieldShellHelper::prefixAndSuffixReserveEditorSpace()
{
    const Fixture fixture;
    const TextFieldSpec spec = fixture.outlinedSpec();

    const auto base = layoutFor(spec,
                                fixture.theme,
                                QtMaterialTextFieldShellHelper::Variant::Outlined,
                                {},
                                fixture.font);

    QtMaterialTextFieldShellHelper::Accessories accessories;
    accessories.prefixText = QStringLiteral("https://");
    accessories.suffixText = QStringLiteral(".com");

    const auto withAccessories = layoutFor(spec,
                                           fixture.theme,
                                           QtMaterialTextFieldShellHelper::Variant::Outlined,
                                           accessories,
                                           fixture.font);

    QVERIFY(!withAccessories.prefixRect.isEmpty());
    QVERIFY(!withAccessories.suffixRect.isEmpty());
    QVERIFY(withAccessories.editorRect.width() < base.editorRect.width());
    QVERIFY(withAccessories.prefixRect.right() < withAccessories.editorRect.left());
    QVERIFY(withAccessories.suffixRect.left() > withAccessories.editorRect.right());
}

void tst_TextFieldShellHelper::leadingAndTrailingIconsReserveEditorSpace()
{
    const Fixture fixture;
    const TextFieldSpec spec = fixture.outlinedSpec();

    const auto base = layoutFor(spec,
                                fixture.theme,
                                QtMaterialTextFieldShellHelper::Variant::Outlined,
                                {},
                                fixture.font);

    QtMaterialTextFieldShellHelper::Accessories accessories;
    accessories.hasLeadingIcon = true;
    accessories.hasTrailingIcon = true;

    const auto withIcons = layoutFor(spec,
                                     fixture.theme,
                                     QtMaterialTextFieldShellHelper::Variant::Outlined,
                                     accessories,
                                     fixture.font);

    QVERIFY(!withIcons.leadingIconRect.isEmpty());
    QVERIFY(!withIcons.trailingIconRect.isEmpty());
    QVERIFY(withIcons.editorRect.width() < base.editorRect.width());
    QVERIFY(withIcons.leadingIconRect.right() < withIcons.editorRect.left());
    QVERIFY(withIcons.trailingIconRect.left() > withIcons.editorRect.right());
}

void tst_TextFieldShellHelper::clearActionUsesEndActionRectAndSuppressesTrailingIcon()
{
    const Fixture fixture;
    const TextFieldSpec spec = fixture.outlinedSpec();

    QtMaterialTextFieldShellHelper::Accessories accessories;
    accessories.hasTrailingIcon = true;
    accessories.endActionMode = QtMaterialTextFieldShellHelper::EndActionMode::ClearText;

    const auto layout = layoutFor(spec,
                                  fixture.theme,
                                  QtMaterialTextFieldShellHelper::Variant::Outlined,
                                  accessories,
                                  fixture.font);

    QVERIFY(!layout.endActionRect.isEmpty());
    QVERIFY(layout.trailingIconRect.isEmpty());
    QVERIFY(layout.endActionRect.left() > layout.editorRect.right());
}

void tst_TextFieldShellHelper::passwordToggleUsesEndActionRect()
{
    const Fixture fixture;
    const TextFieldSpec spec = fixture.outlinedSpec();

    QtMaterialTextFieldShellHelper::Accessories accessories;
    accessories.endActionMode = QtMaterialTextFieldShellHelper::EndActionMode::TogglePasswordVisibility;

    const auto hiddenLayout = layoutFor(spec,
                                        fixture.theme,
                                        QtMaterialTextFieldShellHelper::Variant::Outlined,
                                        accessories,
                                        fixture.font);

    accessories.passwordVisible = true;
    const auto visibleLayout = layoutFor(spec,
                                         fixture.theme,
                                         QtMaterialTextFieldShellHelper::Variant::Outlined,
                                         accessories,
                                         fixture.font);

    QVERIFY(!hiddenLayout.endActionRect.isEmpty());
    QVERIFY(!visibleLayout.endActionRect.isEmpty());
    QCOMPARE(hiddenLayout.endActionRect.height(), visibleLayout.endActionRect.height());
    QVERIFY(hiddenLayout.editorRect.width() > 0);
    QVERIFY(visibleLayout.editorRect.width() > 0);
}

void tst_TextFieldShellHelper::rtlMirrorsLeadingSideAccessories()
{
    const Fixture fixture;
    const TextFieldSpec spec = fixture.outlinedSpec();

    QtMaterialTextFieldShellHelper::Accessories accessories;
    accessories.hasLeadingIcon = true;
    accessories.prefixText = QStringLiteral("EUR");

    const auto ltr = layoutFor(spec,
                               fixture.theme,
                               QtMaterialTextFieldShellHelper::Variant::Outlined,
                               accessories,
                               fixture.font);

    accessories.layoutDirection = Qt::RightToLeft;
    const auto rtl = layoutFor(spec,
                               fixture.theme,
                               QtMaterialTextFieldShellHelper::Variant::Outlined,
                               accessories,
                               fixture.font);

    QVERIFY(ltr.leadingIconRect.right() < ltr.editorRect.left());
    QVERIFY(ltr.prefixRect.right() < ltr.editorRect.left());
    QVERIFY(rtl.leadingIconRect.left() > rtl.editorRect.right());
    QVERIFY(rtl.prefixRect.left() > rtl.editorRect.right());
}

void tst_TextFieldShellHelper::smallWidthDoesNotProduceNegativeRects()
{
    const Fixture fixture;
    const TextFieldSpec spec = fixture.outlinedSpec();

    QtMaterialTextFieldShellHelper::Accessories accessories;
    accessories.hasLeadingIcon = true;
    accessories.hasTrailingIcon = true;
    accessories.prefixText = QStringLiteral("LONGPREFIX");
    accessories.suffixText = QStringLiteral("LONGSUFFIX");
    accessories.endActionMode = QtMaterialTextFieldShellHelper::EndActionMode::CustomTrailingAction;
    accessories.customEndActionText = QStringLiteral("Open");

    const auto layout = layoutFor(spec,
                                  fixture.theme,
                                  QtMaterialTextFieldShellHelper::Variant::Outlined,
                                  accessories,
                                  fixture.font,
                                  96);

    QVERIFY(layout.containerRect.width() >= 0);
    QVERIFY(layout.editorRect.width() >= 0);
    QVERIFY(layout.leadingIconRect.width() >= 0);
    QVERIFY(layout.prefixRect.width() >= 0);
    QVERIFY(layout.suffixRect.width() >= 0);
    QVERIFY(layout.endActionRect.width() >= 0);
}

void tst_TextFieldShellHelper::filledVariantOffsetsEditorDown()
{
    const Fixture fixture;
    const TextFieldSpec spec = fixture.filledSpec();

    const auto outlined = layoutFor(spec,
                                    fixture.theme,
                                    QtMaterialTextFieldShellHelper::Variant::Outlined,
                                    {},
                                    fixture.font);
    const auto filled = layoutFor(spec,
                                  fixture.theme,
                                  QtMaterialTextFieldShellHelper::Variant::Filled,
                                  {},
                                  fixture.font);

    QVERIFY(filled.editorRect.top() > outlined.editorRect.top());
    QCOMPARE(filled.containerRect, outlined.containerRect);
}

QTEST_MAIN(tst_TextFieldShellHelper)
#include "tst_textfieldshellhelper.moc"

