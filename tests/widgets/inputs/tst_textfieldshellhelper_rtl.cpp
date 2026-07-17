#include <QtTest/QtTest>

#include <QFont>
#include <QRect>

#include "qtmaterialtextfieldshellhelper_p.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

using QtMaterial::QtMaterialTextFieldShellHelper;
using QtMaterial::TextFieldSpec;
using QtMaterial::Theme;

class tst_TextFieldShellHelperRtl : public QObject {
    Q_OBJECT

private slots:
    void layoutStoresDirection();
    void rtlMirrorsLeadingAndTrailingAccessories();
    void editorRectNeverBecomesNegativeForDenseAccessories();

private:
    static TextFieldSpec spec();
    static Theme theme();
    static QtMaterialTextFieldShellHelper::Accessories accessories(Qt::LayoutDirection direction);
};

TextFieldSpec tst_TextFieldShellHelperRtl::spec()
{
    TextFieldSpec s;
    s.minHeight = 56;
    s.topLabelHeight = 20;
    s.supportingHeight = 20;
    s.supportingTopSpacing = 4;
    s.horizontalPadding = 16;
    s.verticalPadding = 8;
    return s;
}

Theme tst_TextFieldShellHelperRtl::theme()
{
    return Theme();
}

QtMaterialTextFieldShellHelper::Accessories tst_TextFieldShellHelperRtl::accessories(
    Qt::LayoutDirection direction)
{
    QtMaterialTextFieldShellHelper::Accessories a;
    a.layoutDirection = direction;
    a.hasLeadingIcon = true;
    a.hasTrailingIcon = true;
    a.prefixText = QStringLiteral("EUR");
    a.suffixText = QStringLiteral("TTC");
    a.iconExtent = 18;
    a.accessorySpacing = 8;
    a.accessoryTextPadding = 4;
    a.minimumAccessoryTextWidth = 24;
    return a;
}

void tst_TextFieldShellHelperRtl::layoutStoresDirection()
{
    const auto layout = QtMaterialTextFieldShellHelper::layoutFor(
        QRect(0, 0, 320, 100),
        spec(),
        QtMaterialTextFieldShellHelper::Variant::Outlined,
        accessories(Qt::RightToLeft),
        QFont());

    QCOMPARE(layout.layoutDirection, Qt::RightToLeft);
}

void tst_TextFieldShellHelperRtl::rtlMirrorsLeadingAndTrailingAccessories()
{
    const auto ltr = QtMaterialTextFieldShellHelper::layoutFor(
        QRect(0, 0, 360, 100),
        spec(),
        QtMaterialTextFieldShellHelper::Variant::Outlined,
        accessories(Qt::LeftToRight),
        QFont());

    const auto rtl = QtMaterialTextFieldShellHelper::layoutFor(
        QRect(0, 0, 360, 100),
        spec(),
        QtMaterialTextFieldShellHelper::Variant::Outlined,
        accessories(Qt::RightToLeft),
        QFont());

    QVERIFY(ltr.leadingIconRect.isValid());
    QVERIFY(ltr.trailingIconRect.isValid());
    QVERIFY(rtl.leadingIconRect.isValid());
    QVERIFY(rtl.trailingIconRect.isValid());

    QVERIFY2(ltr.leadingIconRect.right() < ltr.editorRect.left(),
             "LTR leading icon should reserve space before the editor rect.");
    QVERIFY2(ltr.trailingIconRect.left() > ltr.editorRect.right(),
             "LTR trailing icon should reserve space after the editor rect.");

    QVERIFY2(rtl.leadingIconRect.left() > rtl.editorRect.right(),
             "RTL leading icon should move to the visual right side.");
    QVERIFY2(rtl.trailingIconRect.right() < rtl.editorRect.left(),
             "RTL trailing icon should move to the visual left side.");
}

void tst_TextFieldShellHelperRtl::editorRectNeverBecomesNegativeForDenseAccessories()
{
    auto a = accessories(Qt::RightToLeft);
    a.prefixText = QStringLiteral("very-long-prefix-value");
    a.suffixText = QStringLiteral("very-long-suffix-value");
    a.endActionMode = QtMaterialTextFieldShellHelper::EndActionMode::CustomTrailingAction;
    a.customEndActionText = QStringLiteral("Action");
    a.hasCustomEndActionIcon = true;

    const auto layout = QtMaterialTextFieldShellHelper::layoutFor(
        QRect(0, 0, 180, 100),
        spec(),
        QtMaterialTextFieldShellHelper::Variant::Outlined,
        a,
        QFont());

    QVERIFY(layout.editorRect.width() >= 0);
    QVERIFY(layout.editorRect.height() >= 0);
    QCOMPARE(layout.layoutDirection, Qt::RightToLeft);
}

QTEST_MAIN(tst_TextFieldShellHelperRtl)
#include "tst_textfieldshellhelper_rtl.moc"
