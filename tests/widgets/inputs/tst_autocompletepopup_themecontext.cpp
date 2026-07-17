#include <QLineEdit>
#include <QSignalSpy>
#include <QtTest/QtTest>

#include "qtmaterial/core/qtmaterialwidget.h"
#include "qtmaterial/specs/qtmaterialautocompletepopupspec.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"
#include "qtmaterial/widgets/inputs/qtmaterialautocompletepopup.h"

using namespace QtMaterial;

namespace {

Theme makeTheme(const QString& seed, bool dark = false)
{
    ThemeBuilder builder;
    return dark
        ? builder.buildDarkFromSeed(QColor(seed))
        : builder.buildLightFromSeed(QColor(seed));
}

} // namespace

class tst_AutocompletePopupThemeContext : public QObject
{
    Q_OBJECT

private slots:
    void explicitContextResolvesSpec();
    void inheritsContextFromAnchor();
    void repeatedVisibilityRequestRecoversPopup();
};

void tst_AutocompletePopupThemeContext::explicitContextResolvesSpec()
{
    ThemeContext context(
        makeTheme(QStringLiteral("#6750A4")));
    QtMaterialAutocompletePopup popup;

    popup.setThemeContext(&context);

    QCOMPARE(popup.themeContext(), &context);
    QCOMPARE(popup.effectiveThemeContext(), &context);
    QCOMPARE(
        popup.resolvedSpec().containerColor,
        context.theme().colorScheme().color(
            ColorRole::SurfaceContainerHigh));
}

void tst_AutocompletePopupThemeContext::inheritsContextFromAnchor()
{
    ThemeContext context(
        makeTheme(QStringLiteral("#146C2E"), true));
    QtMaterialWidget host;
    host.setThemeContext(&context);
    QLineEdit anchor(&host);

    QtMaterialAutocompletePopup popup;
    popup.setAnchorLineEdit(&anchor);

    QCOMPARE(
        popup.effectiveThemeContext(),
        &context);
    QCOMPARE(
        popup.resolvedSpec().containerColor,
        context.theme().colorScheme().color(
            ColorRole::SurfaceContainerHigh));
}

void tst_AutocompletePopupThemeContext::
repeatedVisibilityRequestRecoversPopup()
{
    QWidget host;
    QLineEdit anchor(&host);
    host.show();

    QtMaterialAutocompletePopup popup;
    popup.setAnchorLineEdit(&anchor);
    popup.setSuggestions(
        {QStringLiteral("alpha")});

    popup.setPopupVisible(true);
    QCoreApplication::processEvents();
    QVERIFY(popup.isPopupVisible());

    popup.hide();
    QCoreApplication::processEvents();
    QVERIFY(!popup.isPopupVisible());

    popup.setPopupVisible(true);
    QCoreApplication::processEvents();
    QVERIFY(popup.isPopupVisible());

    popup.setPopupVisible(false);
    QCoreApplication::processEvents();
    QVERIFY(!popup.isPopupVisible());
}

QTEST_MAIN(tst_AutocompletePopupThemeContext)

#include "tst_autocompletepopup_themecontext.moc"
