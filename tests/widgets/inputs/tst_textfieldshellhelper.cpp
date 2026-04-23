#include <QtTest/QtTest>

#include <QFont>
#include <QFontMetrics>
#include <QRect>
#include <QString>

#include "qtmaterial/core/qtmaterialdensity.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "qtmaterial/theme/qtmaterialtheme.h"
#include "qtmaterialtextfieldshellhelper_p.h"

namespace {

QtMaterial::Theme makeTheme()
{
    return QtMaterial::Theme();
}

QtMaterial::TextFieldSpec makeOutlinedSpec()
{
    QtMaterial::SpecFactory factory;
    return factory.outlinedTextFieldSpec(makeTheme(), QtMaterial::Density::Default);
}

QtMaterial::TextFieldSpec makeFilledSpec()
{
    QtMaterial::SpecFactory factory;
    return factory.filledTextFieldSpec(makeTheme(), QtMaterial::Density::Default);
}

QFont makeFont()
{
    return QFont();
}

} // namespace

class tst_TextFieldShellHelper : public QObject
{
    Q_OBJECT

private slots:
    void outlinedLayoutBaseContract();
    void accessoriesShrinkEditorAndCreateSlotsLtr();
    void accessoriesMirrorInRtl();
    void filledVariantPushesEditorLowerThanOutlined();
    void elidedTextIncludesAccessoryStrings();
    void passwordEndActionProducesActionText();
};

void tst_TextFieldShellHelper::outlinedLayoutBaseContract()
{
    const QRect bounds(0, 0, 360, 92);
    const QtMaterial::Theme theme = makeTheme();
    const QtMaterial::TextFieldSpec spec = makeOutlinedSpec();
    const QFont font = makeFont();

    QtMaterial::QtMaterialTextFieldShellHelper::Accessories accessories;

    const QtMaterial::QtMaterialTextFieldShellHelper::Layout layout =
        QtMaterial::QtMaterialTextFieldShellHelper::layoutFor(
            bounds,
            spec,
            theme,
            QtMaterial::QtMaterialTextFieldShellHelper::Variant::Outlined,
            accessories,
            font);

    QVERIFY(!layout.containerRect.isEmpty());
    QVERIFY(!layout.labelRect.isEmpty());
    QVERIFY(!layout.editorRect.isEmpty());
    QVERIFY(!layout.supportingRect.isEmpty());
    QVERIFY(!layout.focusRect.isEmpty());

    QVERIFY(layout.containerRect.top() >= spec.topLabelHeight);
    QVERIFY(layout.editorRect.left() >= layout.containerRect.left());
    QVERIFY(layout.editorRect.right() <= layout.containerRect.right());
    QVERIFY(layout.editorRect.top() >= layout.containerRect.top());
    QVERIFY(layout.editorRect.bottom() <= layout.containerRect.bottom());

    QVERIFY(layout.leadingIconRect.isEmpty());
    QVERIFY(layout.prefixRect.isEmpty());
    QVERIFY(layout.suffixRect.isEmpty());
    QVERIFY(layout.trailingIconRect.isEmpty());
    QVERIFY(layout.endActionRect.isEmpty());
}

void tst_TextFieldShellHelper::accessoriesShrinkEditorAndCreateSlotsLtr()
{
    const QRect bounds(0, 0, 360, 92);
    const QtMaterial::Theme theme = makeTheme();
    const QtMaterial::TextFieldSpec spec = makeOutlinedSpec();
    const QFont font = makeFont();

    QtMaterial::QtMaterialTextFieldShellHelper::Accessories none;

    const QtMaterial::QtMaterialTextFieldShellHelper::Layout baseLayout =
        QtMaterial::QtMaterialTextFieldShellHelper::layoutFor(
            bounds,
            spec,
            theme,
            QtMaterial::QtMaterialTextFieldShellHelper::Variant::Outlined,
            none,
            font);

    QtMaterial::QtMaterialTextFieldShellHelper::Accessories accessories;
    accessories.hasLeadingIcon = true;
    accessories.prefixText = QStringLiteral("https://");
    accessories.suffixText = QStringLiteral(".com");
    accessories.endActionMode =
        QtMaterial::QtMaterialTextFieldShellHelper::EndActionMode::ClearText;
    accessories.layoutDirection = Qt::LeftToRight;
    accessories.iconExtent = 18;
    accessories.accessorySpacing = 8;
    accessories.accessoryTextPadding = 4;
    accessories.minimumAccessoryTextWidth = 24;

    const QtMaterial::QtMaterialTextFieldShellHelper::Layout layout =
        QtMaterial::QtMaterialTextFieldShellHelper::layoutFor(
            bounds,
            spec,
            theme,
            QtMaterial::QtMaterialTextFieldShellHelper::Variant::Outlined,
            accessories,
            font);

    QVERIFY(layout.editorRect.width() < baseLayout.editorRect.width());

    QVERIFY(!layout.leadingIconRect.isEmpty());
    QVERIFY(!layout.prefixRect.isEmpty());
    QVERIFY(!layout.suffixRect.isEmpty());
    QVERIFY(layout.trailingIconRect.isEmpty());
    QVERIFY(!layout.endActionRect.isEmpty());

    QVERIFY(layout.leadingIconRect.right() < layout.prefixRect.left());
    QVERIFY(layout.prefixRect.right() < layout.editorRect.left());
    QVERIFY(layout.editorRect.right() < layout.suffixRect.left());
    QVERIFY(layout.suffixRect.right() < layout.endActionRect.left());
}

void tst_TextFieldShellHelper::accessoriesMirrorInRtl()
{
    const QRect bounds(0, 0, 360, 92);
    const QtMaterial::Theme theme = makeTheme();
    const QtMaterial::TextFieldSpec spec = makeOutlinedSpec();
    const QFont font = makeFont();

    QtMaterial::QtMaterialTextFieldShellHelper::Accessories accessories;
    accessories.hasLeadingIcon = true;
    accessories.prefixText = QStringLiteral("EUR");
    accessories.suffixText = QStringLiteral("TTC");
    accessories.endActionMode =
        QtMaterial::QtMaterialTextFieldShellHelper::EndActionMode::TogglePasswordVisibility;
    accessories.passwordVisible = false;
    accessories.layoutDirection = Qt::RightToLeft;
    accessories.iconExtent = 18;
    accessories.accessorySpacing = 8;
    accessories.accessoryTextPadding = 4;
    accessories.minimumAccessoryTextWidth = 24;

    const QtMaterial::QtMaterialTextFieldShellHelper::Layout layout =
        QtMaterial::QtMaterialTextFieldShellHelper::layoutFor(
            bounds,
            spec,
            theme,
            QtMaterial::QtMaterialTextFieldShellHelper::Variant::Outlined,
            accessories,
            font);

    QVERIFY(!layout.leadingIconRect.isEmpty());
    QVERIFY(!layout.prefixRect.isEmpty());
    QVERIFY(!layout.suffixRect.isEmpty());
    QVERIFY(!layout.endActionRect.isEmpty());

    QVERIFY(layout.editorRect.right() < layout.prefixRect.left());
    QVERIFY(layout.prefixRect.right() < layout.leadingIconRect.left());

    QVERIFY(layout.endActionRect.right() < layout.editorRect.left());
    QVERIFY(layout.suffixRect.right() < layout.endActionRect.left());
}

void tst_TextFieldShellHelper::filledVariantPushesEditorLowerThanOutlined()
{
    const QRect bounds(0, 0, 360, 92);
    const QtMaterial::Theme theme = makeTheme();
    const QtMaterial::TextFieldSpec outlinedSpec = makeOutlinedSpec();
    const QtMaterial::TextFieldSpec filledSpec = makeFilledSpec();
    const QFont font = makeFont();

    QtMaterial::QtMaterialTextFieldShellHelper::Accessories accessories;

    const QtMaterial::QtMaterialTextFieldShellHelper::Layout outlinedLayout =
        QtMaterial::QtMaterialTextFieldShellHelper::layoutFor(
            bounds,
            outlinedSpec,
            theme,
            QtMaterial::QtMaterialTextFieldShellHelper::Variant::Outlined,
            accessories,
            font);

    const QtMaterial::QtMaterialTextFieldShellHelper::Layout filledLayout =
        QtMaterial::QtMaterialTextFieldShellHelper::layoutFor(
            bounds,
            filledSpec,
            theme,
            QtMaterial::QtMaterialTextFieldShellHelper::Variant::Filled,
            accessories,
            font);

    QVERIFY(filledLayout.editorRect.top() >= outlinedLayout.editorRect.top());
    QVERIFY(filledLayout.containerRect.top() == outlinedLayout.containerRect.top()
            || filledLayout.containerRect.top() >= 0);
}

void tst_TextFieldShellHelper::elidedTextIncludesAccessoryStrings()
{
    const QRect bounds(0, 0, 220, 92);
    const QtMaterial::Theme theme = makeTheme();
    const QtMaterial::TextFieldSpec spec = makeOutlinedSpec();
    const QFont font = makeFont();
    const QFontMetrics metrics(font);

    QtMaterial::QtMaterialTextFieldShellHelper::Accessories accessories;
    accessories.prefixText = QStringLiteral("very-long-prefix-text");
    accessories.suffixText = QStringLiteral("very-long-suffix-text");
    accessories.layoutDirection = Qt::LeftToRight;
    accessories.iconExtent = 18;
    accessories.accessorySpacing = 8;
    accessories.accessoryTextPadding = 4;
    accessories.minimumAccessoryTextWidth = 24;

    const QtMaterial::QtMaterialTextFieldShellHelper::Layout layout =
        QtMaterial::QtMaterialTextFieldShellHelper::layoutFor(
            bounds,
            spec,
            theme,
            QtMaterial::QtMaterialTextFieldShellHelper::Variant::Outlined,
            accessories,
            font);

    const QtMaterial::QtMaterialTextFieldShellHelper::ElidedText text =
        QtMaterial::QtMaterialTextFieldShellHelper::elidedTextFor(
            layout,
            spec,
            accessories,
            QStringLiteral("Very long label that should be elided"),
            QStringLiteral("Very long supporting text that should be elided too"),
            QStringLiteral("Very long error text that should be elided too"),
            font);

    QVERIFY(!text.labelText.isEmpty());
    QVERIFY(!text.supportingText.isEmpty());
    QVERIFY(!text.errorText.isEmpty());
    QVERIFY(!text.prefixText.isEmpty());
    QVERIFY(!text.suffixText.isEmpty());

    QVERIFY(metrics.horizontalAdvance(text.labelText) <= qMax(0, layout.labelRect.width()));
    QVERIFY(metrics.horizontalAdvance(text.supportingText)
            <= qMax(0, layout.supportingRect.width()));
    QVERIFY(metrics.horizontalAdvance(text.errorText)
            <= qMax(0, layout.supportingRect.width()));
    QVERIFY(metrics.horizontalAdvance(text.prefixText) <= qMax(0, layout.prefixRect.width()));
    QVERIFY(metrics.horizontalAdvance(text.suffixText) <= qMax(0, layout.suffixRect.width()));
}

void tst_TextFieldShellHelper::passwordEndActionProducesActionText()
{
    const QRect bounds(0, 0, 260, 92);
    const QtMaterial::Theme theme = makeTheme();
    const QtMaterial::TextFieldSpec spec = makeOutlinedSpec();
    const QFont font = makeFont();
    const QFontMetrics metrics(font);

    QtMaterial::QtMaterialTextFieldShellHelper::Accessories accessories;
    accessories.endActionMode =
        QtMaterial::QtMaterialTextFieldShellHelper::EndActionMode::TogglePasswordVisibility;
    accessories.passwordVisible = false;
    accessories.layoutDirection = Qt::LeftToRight;
    accessories.iconExtent = 18;
    accessories.accessorySpacing = 8;
    accessories.accessoryTextPadding = 4;
    accessories.minimumAccessoryTextWidth = 24;

    const QtMaterial::QtMaterialTextFieldShellHelper::Layout layoutShow =
        QtMaterial::QtMaterialTextFieldShellHelper::layoutFor(
            bounds,
            spec,
            theme,
            QtMaterial::QtMaterialTextFieldShellHelper::Variant::Outlined,
            accessories,
            font);

    const QtMaterial::QtMaterialTextFieldShellHelper::ElidedText textShow =
        QtMaterial::QtMaterialTextFieldShellHelper::elidedTextFor(
            layoutShow,
            spec,
            accessories,
            QStringLiteral("Password"),
            QString(),
            QString(),
            font);

    QVERIFY(!layoutShow.endActionRect.isEmpty());
    QVERIFY(!textShow.endActionText.isEmpty());
    QVERIFY(metrics.horizontalAdvance(textShow.endActionText)
            <= qMax(0, layoutShow.endActionRect.width()));

    accessories.passwordVisible = true;

    const QtMaterial::QtMaterialTextFieldShellHelper::Layout layoutHide =
        QtMaterial::QtMaterialTextFieldShellHelper::layoutFor(
            bounds,
            spec,
            theme,
            QtMaterial::QtMaterialTextFieldShellHelper::Variant::Outlined,
            accessories,
            font);

    const QtMaterial::QtMaterialTextFieldShellHelper::ElidedText textHide =
        QtMaterial::QtMaterialTextFieldShellHelper::elidedTextFor(
            layoutHide,
            spec,
            accessories,
            QStringLiteral("Password"),
            QString(),
            QString(),
            font);

    QVERIFY(!textHide.endActionText.isEmpty());
    QVERIFY(textHide.endActionText != textShow.endActionText);
}

QTEST_MAIN(tst_TextFieldShellHelper)
#include "tst_textfieldshellhelper.moc"