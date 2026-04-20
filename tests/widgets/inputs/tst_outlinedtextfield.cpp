#include <QtTest/QtTest>
#include <QLineEdit>
#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"

class tst_Outlinedtextfield : public QObject
{
    Q_OBJECT
private slots:
    void basicConstruction();
    void accessibilityState();
};

void tst_Outlinedtextfield::basicConstruction()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
    widget.setLabelText(QStringLiteral("Label"));
    QVERIFY(widget.lineEdit() != nullptr);
    QCOMPARE(widget.focusProxy(), widget.lineEdit());
}

void tst_Outlinedtextfield::accessibilityState()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
    widget.setLabelText(QStringLiteral("Email"));
    widget.setSupportingText(QStringLiteral("We will not spam you"));
    QCOMPARE(widget.accessibleName(), QStringLiteral("Email"));
    QCOMPARE(widget.lineEdit()->accessibleName(), QStringLiteral("Email"));
    QCOMPARE(widget.lineEdit()->accessibleDescription(), QStringLiteral("We will not spam you"));

    widget.setErrorText(QStringLiteral("Invalid email"));
    widget.setHasErrorState(true);
    QCOMPARE(widget.lineEdit()->accessibleDescription(), QStringLiteral("Invalid email"));
}

QTEST_MAIN(tst_Outlinedtextfield)
#include "tst_outlinedtextfield.moc"
