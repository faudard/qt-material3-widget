#include <QtTest/QtTest>

// Focused scaffold for validation-pattern tests.
// Wire this once the shared input shell exposes validator + validation state APIs.

class tst_TextFieldValidationPatterns : public QObject
{
    Q_OBJECT

private slots:
    void manualValidation();
    void blurValidation();
    void compositeValidatorStopsOnError();
};

void tst_TextFieldValidationPatterns::manualValidation()
{
    QSKIP("Scaffold only: validation-pattern API not wired yet.");
}

void tst_TextFieldValidationPatterns::blurValidation()
{
    QSKIP("Scaffold only: validation-pattern API not wired yet.");
}

void tst_TextFieldValidationPatterns::compositeValidatorStopsOnError()
{
    QSKIP("Scaffold only: validation-pattern API not wired yet.");
}

QTEST_MAIN(tst_TextFieldValidationPatterns)
#include "tst_textfield_validation_patterns.moc"
