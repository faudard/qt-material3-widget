#include <QtTest/QtTest>

#include "qtmaterial/widgets/inputs/common/qtmaterialformfieldcontrol.h"

class tst_FormFieldControl : public QObject {
    Q_OBJECT
private slots:
    void exposesCommonFormProperties()
    {
        QtMaterial::QtMaterialFormFieldControl field;
        QSignalSpy labelSpy(&field, &QtMaterial::QtMaterialFormFieldControl::labelChanged);
        field.setLabel(QStringLiteral("Email"));
        field.setHelperText(QStringLiteral("Work email"));
        field.setErrorText(QStringLiteral("Invalid email"));
        field.setRequired(true);
        field.setInvalid(true);
        field.setReadOnly(true);

        QCOMPARE(field.label(), QStringLiteral("Email"));
        QCOMPARE(field.helperText(), QStringLiteral("Work email"));
        QCOMPARE(field.errorText(), QStringLiteral("Invalid email"));
        QVERIFY(field.isRequired());
        QVERIFY(field.isInvalid());
        QVERIFY(field.isReadOnly());
        QCOMPARE(labelSpy.count(), 1);
    }
};

QTEST_MAIN(tst_FormFieldControl)
#include "tst_formfieldcontrol.moc"
