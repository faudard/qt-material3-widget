#include <QtTest/QtTest>

#include <QLineEdit>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

#include "qtmaterial/widgets/inputs/qtmaterialfilledtextfield.h"

class tst_FilledTextField : public QObject
{
    Q_OBJECT

private slots:
    void basicConstruction();
    void constructorSetsLabel();
    void inheritsOutlinedTextFieldPublicApi();
    void inheritedValidationApiWorks();
};

void tst_FilledTextField::basicConstruction()
{
    QtMaterial::QtMaterialFilledTextField widget;

    QVERIFY(widget.lineEdit() != nullptr);
    QCOMPARE(widget.focusProxy(), widget.lineEdit());
    QVERIFY(widget.sizeHint().height() >= 80);
}

void tst_FilledTextField::constructorSetsLabel()
{
    QtMaterial::QtMaterialFilledTextField widget(QStringLiteral("Email"));

    QCOMPARE(widget.labelText(), QStringLiteral("Email"));
    QCOMPARE(widget.accessibleName(), QStringLiteral("Email"));
    QCOMPARE(widget.lineEdit()->accessibleName(), QStringLiteral("Email"));
}

void tst_FilledTextField::inheritsOutlinedTextFieldPublicApi()
{
    QtMaterial::QtMaterialFilledTextField widget;

    widget.setPlaceholderText(QStringLiteral("name@example.com"));
    widget.setPrefixText(QStringLiteral("+33"));
    widget.setSuffixText(QStringLiteral("FR"));
    widget.setClearButtonEnabled(true);
    widget.setEndActionMode(QtMaterial::QtMaterialOutlinedTextField::EndActionMode::ClearText);
    widget.setText(QStringLiteral("612345678"));

    QCOMPARE(widget.placeholderText(), QStringLiteral("name@example.com"));
    QCOMPARE(widget.prefixText(), QStringLiteral("+33"));
    QCOMPARE(widget.suffixText(), QStringLiteral("FR"));
    QVERIFY(widget.isClearButtonEnabled());
    QCOMPARE(widget.endActionMode(), QtMaterial::QtMaterialOutlinedTextField::EndActionMode::ClearText);
    QCOMPARE(widget.text(), QStringLiteral("612345678"));
}

void tst_FilledTextField::inheritedValidationApiWorks()
{
    QtMaterial::QtMaterialFilledTextField widget;
    QRegularExpressionValidator validator(QRegularExpression(QStringLiteral("^[A-Z]{2}$")), &widget);

    widget.setValidator(&validator);
    widget.setValidationFeedbackMode(
        QtMaterial::QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnEdit);

    widget.setText(QStringLiteral("fr"));
    QVERIFY(widget.hasAutomaticValidationError());
    QVERIFY(widget.hasErrorState());

    widget.setText(QStringLiteral("FR"));
    QVERIFY(!widget.hasAutomaticValidationError());
    QVERIFY(!widget.hasErrorState());
}

QTEST_MAIN(tst_FilledTextField)
#include "tst_filledtextfield.moc"
