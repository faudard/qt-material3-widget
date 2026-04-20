#include <QtTest/QtTest>

#include <QLineEdit>

#include "qtmaterial/widgets/inputs/qtmaterialautocompletepopup.h"

class tst_QtMaterialAutocompletePopup : public QObject
{
    Q_OBJECT
private slots:
    void constructs();
    void anchorAndVisibility();
    void suggestionsRoundTrip();
};

void tst_QtMaterialAutocompletePopup::constructs()
{
    QtMaterialAutocompletePopup popup;
    QVERIFY(popup.model() != nullptr);
    QVERIFY(!popup.isPopupVisible());
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

QTEST_MAIN(tst_QtMaterialAutocompletePopup)
#include "tst_autocompletepopup.moc"
