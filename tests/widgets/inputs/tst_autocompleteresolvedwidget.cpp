#include <QtTest>

#include <QLineEdit>

#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"
#include "qtmaterial/widgets/inputs/qtmaterialautocomplete.h"

using namespace QtMaterial;

class tst_AutocompleteResolvedWidget : public QObject
{
    Q_OBJECT

private slots:
    void appliesResolvedInputFontAndMetrics();
    void followsThemeContextChanges();
};

void tst_AutocompleteResolvedWidget::appliesResolvedInputFontAndMetrics()
{
    Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));

    TypographyStyle typography;
    typography.font = QFont(QStringLiteral("Sans Serif"), 16);
    theme.typography().setStyle(TypeRole::BodyLarge, typography);
    theme.shapes().setRadius(ShapeRole::ExtraSmall, 12);

    ThemeContext context(theme);
    QtMaterialAutocomplete field;
    field.setThemeContext(&context);

    const AutocompleteSpec& spec = field.resolvedSpec();
    QCOMPARE(spec.inputFont, typography.font);
    QCOMPARE(field.lineEdit()->font(), typography.font);
    QCOMPARE(spec.inputCornerRadius, 12.0);
    QCOMPARE(field.sizeHint().height(), spec.inputMinHeight);
}

void tst_AutocompleteResolvedWidget::followsThemeContextChanges()
{
    Theme first = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));
    first.shapes().setRadius(ShapeRole::ExtraSmall, 6);

    ThemeContext context(first);
    QtMaterialAutocomplete field;
    field.setThemeContext(&context);

    QCOMPARE(field.resolvedSpec().inputCornerRadius, 6.0);

    Theme second = ThemeBuilder().buildDarkFromSeed(
        QColor(QStringLiteral("#006874")));
    second.shapes().setRadius(ShapeRole::ExtraSmall, 18);
    second.stateLayer().pressOpacity = 0.23;
    QVERIFY(context.setTheme(second));

    QCOMPARE(field.resolvedSpec().inputCornerRadius, 18.0);
    QCOMPARE(field.resolvedSpec().pressStateLayerOpacity, 0.23);
}

QTEST_MAIN(tst_AutocompleteResolvedWidget)
#include "tst_autocompleteresolvedwidget.moc"
