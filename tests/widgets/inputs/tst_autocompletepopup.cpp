#include <QtTest/QtTest>

#include <QLineEdit>

#include "qtmaterial/core/qtmaterialwidget.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"
#include "qtmaterialautocompletepopup_p.h"

class tst_QtMaterialAutocompletePopup : public QObject
{
    Q_OBJECT
private slots:
    void themeContextUsesBinding();
    void constructs();
    void constructsWithVisibleParent();
    void anchorAndVisibility();
    void suggestionsRoundTrip();
};

void tst_QtMaterialAutocompletePopup::constructs()
{
    QtMaterialAutocompletePopup popup;
    QVERIFY(popup.model() != nullptr);
    QVERIFY(!popup.isPopupVisible());
}

void tst_QtMaterialAutocompletePopup::constructsWithVisibleParent()
{
    QtMaterial::ThemeContext context;
    QtMaterial::QtMaterialWidget host;
    host.setThemeContext(&context);
    host.show();
    QCoreApplication::processEvents();

    QtMaterialAutocompletePopup popup(&host);
    QCOMPARE(popup.windowType(), Qt::Popup);
    QCOMPARE(popup.effectiveThemeContext(), &context);
    QVERIFY(popup.model() != nullptr);
    QVERIFY(popup.sizeHint().isValid());
    QVERIFY(popup.minimumSizeHint().isValid());
    QVERIFY(!popup.isVisible());
}

void tst_QtMaterialAutocompletePopup::anchorAndVisibility()
{
    QWidget host;
    QLineEdit edit(&host);
    host.show();

    QtMaterialAutocompletePopup popup;
    popup.setAnchorLineEdit(&edit);
    QCOMPARE(popup.anchorLineEdit(), &edit);

    popup.setPopupVisible(true);
    QVERIFY(popup.isPopupVisible());

    popup.setPopupVisible(false);
    QVERIFY(!popup.isPopupVisible());
}

void tst_QtMaterialAutocompletePopup::suggestionsRoundTrip()
{
    QtMaterialAutocompletePopup popup;
    popup.setSuggestions({QStringLiteral("alpha"), QStringLiteral("beta")});
    QVERIFY(popup.model() != nullptr);
}


void tst_QtMaterialAutocompletePopup::
    themeContextUsesBinding()
{
    QtMaterial::QtMaterialWidget host;
    QtMaterial::ThemeContext inheritedContext;
    host.setThemeContext(&inheritedContext);

    QtMaterialAutocompletePopup popup(&host);
    QCOMPARE(
        popup.effectiveThemeContext(),
        &inheritedContext);

    QtMaterial::ThemeContext explicitContext;
    popup.setThemeContext(&explicitContext);

    QCOMPARE(
        popup.themeContext(),
        &explicitContext);
    QCOMPARE(
        popup.effectiveThemeContext(),
        &explicitContext);

    popup.setThemeContext(nullptr);

    QCOMPARE(
        popup.themeContext(),
        nullptr);
    QCOMPARE(
        popup.effectiveThemeContext(),
        &inheritedContext);
}

QTEST_MAIN(tst_QtMaterialAutocompletePopup)
#include "tst_autocompletepopup.moc"
