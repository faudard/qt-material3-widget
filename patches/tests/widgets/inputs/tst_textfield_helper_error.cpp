#include <QtTest/QtTest>

#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"
#include "qtmaterial/widgets/inputs/qtmaterialfilledtextfield.h"

class tst_TextFieldHelperError : public QObject
{
    Q_OBJECT

private slots:
    void outlinedPrefersErrorText();
    void filledReservesSupportingLine();
};

void tst_TextFieldHelperError::outlinedPrefersErrorText()
{
    QtMaterialOutlinedTextField field;
    field.setSupportingText(QStringLiteral("Helper text"));
    field.setErrorText(QStringLiteral("Error text"));
    field.setHasError(true);

    QVERIFY(field.errorText() == QStringLiteral("Error text"));
}

void tst_TextFieldHelperError::filledReservesSupportingLine()
{
    QtMaterialFilledTextField field(QStringLiteral("Label"));
    field.setSupportingText(QString());

    const QSize hint = field.sizeHint();
    QVERIFY(hint.height() > 0);
}

QTEST_MAIN(tst_TextFieldHelperError)
#include "tst_textfield_helper_error.moc"
