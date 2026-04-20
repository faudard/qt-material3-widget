#include <QtTest/QtTest>

// Focused scaffold for validation render baselines.
// Intended snapshots:
// - neutral helper state
// - warning state
// - success state
// - error state

class tst_TextFieldValidationRender : public QObject
{
    Q_OBJECT

private slots:
    void outlinedValidationStates();
    void filledValidationStates();
    void dateFieldValidationStates();
};

void tst_TextFieldValidationRender::outlinedValidationStates()
{
    QSKIP("Scaffold only: render baseline system not wired yet.");
}

void tst_TextFieldValidationRender::filledValidationStates()
{
    QSKIP("Scaffold only: render baseline system not wired yet.");
}

void tst_TextFieldValidationRender::dateFieldValidationStates()
{
    QSKIP("Scaffold only: render baseline system not wired yet.");
}

QTEST_MAIN(tst_TextFieldValidationRender)
#include "tst_textfield_validation_render.moc"
