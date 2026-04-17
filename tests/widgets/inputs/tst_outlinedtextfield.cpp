#include <QtTest/QtTest>
#include <QLineEdit>
#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"

class tst_Outlinedtextfield : public QObject
{
    Q_OBJECT
private slots:
    void basicConstruction();
    void textRoundTrip();
    void sizingLooksReasonable();
};

void tst_Outlinedtextfield::basicConstruction()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
    widget.setLabelText(QStringLiteral("Label"));
    widget.setSupportingText(QStringLiteral("Helper text"));

    QVERIFY(widget.lineEdit() != nullptr);
    QCOMPARE(widget.focusProxy(), widget.lineEdit());
}

void tst_Outlinedtextfield::textRoundTrip()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
    widget.setText(QStringLiteral("hello@example.com"));

    QCOMPARE(widget.text(), QStringLiteral("hello@example.com"));
    QVERIFY(widget.lineEdit() != nullptr);
    QCOMPARE(widget.lineEdit()->text(), QStringLiteral("hello@example.com"));
}

void tst_Outlinedtextfield::sizingLooksReasonable()
{
    QtMaterial::QtMaterialOutlinedTextField widget;
    widget.setLabelText(QStringLiteral("Email"));
    const QSize size = widget.minimumSizeHint();
    QVERIFY(size.width() >= 120);
    QVERIFY(size.height() >= 56);
}

QTEST_MAIN(tst_Outlinedtextfield)
#include "tst_outlinedtextfield.moc"
