#include <QtTest/QtTest>

#include <QLineEdit>

#include "qtmaterial/widgets/inputs/qtmaterialautocompletepopup.h"

class tst_QtMaterialAutocompletePopupRender : public QObject
{
    Q_OBJECT
private slots:
    void smokeRender();
};

void tst_QtMaterialAutocompletePopupRender::smokeRender()
{
    QWidget host;
    QLineEdit edit(&host);
    edit.resize(240, 56);
    host.show();

    QtMaterialAutocompletePopup popup;
    popup.setAnchorLineEdit(&edit);
    popup.setSuggestions({QStringLiteral("Apple"), QStringLiteral("Apricot"), QStringLiteral("Avocado")});
    popup.setPopupVisible(true);
    QVERIFY(popup.isVisible());
}

QTEST_MAIN(tst_QtMaterialAutocompletePopupRender)
#include "tst_autocompletepopup_render.moc"
