#include <QtTest>

#include <QLineEdit>
#include <QPalette>

#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"
#include "qtmaterial/widgets/inputs/qtmaterialautocomplete.h"

using namespace QtMaterial;

class tst_AutocompleteResolvedWidget : public QObject
{
    Q_OBJECT

private slots:
    void appliesThemeToPublicWidgetSurface();
    void followsThemeContextChanges();
};

void tst_AutocompleteResolvedWidget::appliesThemeToPublicWidgetSurface()
{
    Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));

    TypographyStyle typography;
    typography.font = QFont(QStringLiteral("Sans Serif"), 16);
    theme.typography().setStyle(TypeRole::BodyLarge, typography);

    ThemeContext context(theme);
    QtMaterialAutocomplete field;
    field.setThemeContext(&context);

    QVERIFY(field.lineEdit() != nullptr);
    QCOMPARE(
        field.lineEdit()->font().pointSize(),
        typography.font.pointSize());
    QCOMPARE(
        field.lineEdit()->font().weight(),
        typography.font.weight());
    QCOMPARE(
        field.lineEdit()->font().italic(),
        typography.font.italic());

    QCOMPARE(field.sizeHint().width(), 280);
    QCOMPARE(field.minimumSizeHint().width(), 160);
    QVERIFY(field.sizeHint().height() > 0);
    QCOMPARE(
        field.minimumSizeHint().height(),
        field.sizeHint().height());
}

void tst_AutocompleteResolvedWidget::followsThemeContextChanges()
{
    Theme first = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));
    TypographyStyle firstTypography;
    firstTypography.font = QFont(QStringLiteral("Sans Serif"), 13);
    first.typography().setStyle(TypeRole::BodyLarge, firstTypography);

    ThemeContext context(first);
    QtMaterialAutocomplete field;
    field.setThemeContext(&context);

    QVERIFY(field.lineEdit() != nullptr);
    QCOMPARE(
        field.lineEdit()->font().pointSize(),
        firstTypography.font.pointSize());
    const QColor firstTextColor =
        field.lineEdit()->palette().color(QPalette::Text);

    Theme second = ThemeBuilder().buildDarkFromSeed(
        QColor(QStringLiteral("#006874")));
    TypographyStyle secondTypography;
    secondTypography.font = QFont(QStringLiteral("Sans Serif"), 19);
    second.typography().setStyle(TypeRole::BodyLarge, secondTypography);

    QVERIFY(context.setTheme(second));

    QCOMPARE(
        field.lineEdit()->font().pointSize(),
        secondTypography.font.pointSize());
    QCOMPARE(
        field.lineEdit()->font().weight(),
        secondTypography.font.weight());
    QCOMPARE(
        field.lineEdit()->font().italic(),
        secondTypography.font.italic());

    const QColor secondTextColor =
        field.lineEdit()->palette().color(QPalette::Text);
    QVERIFY(secondTextColor.isValid());
    QVERIFY(firstTextColor != secondTextColor);
}

QTEST_MAIN(tst_AutocompleteResolvedWidget)
#include "tst_autocompleteresolvedwidget.moc"
