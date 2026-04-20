
#include <QtTest/QtTest>

#include "qtmaterial/widgets/inputs/qtmaterialdatefield.h"

class tst_DateFieldRender : public QObject
{
    Q_OBJECT

private slots:
    void defaultState();
    void errorState();
};

void tst_DateFieldRender::defaultState()
{
    QtMaterialDateField field(QStringLiteral("Date"));
    field.resize(320, 88);
    field.show();
    QVERIFY(QTest::qWaitForWindowExposed(&field));
    QVERIFY(true); // Replace with snapshot capture in the integrated render harness.
}

void tst_DateFieldRender::errorState()
{
    QtMaterialDateField field(QStringLiteral("Date"));
    field.resize(320, 88);
    if (field.lineEdit()) {
        field.lineEdit()->setText(QStringLiteral("invalid"));
    }
    QMetaObject::invokeMethod(&field, "handleEditorEditingFinished", Qt::DirectConnection);
    field.show();
    QVERIFY(QTest::qWaitForWindowExposed(&field));
    QVERIFY(true); // Replace with snapshot capture in the integrated render harness.
}

QTEST_MAIN(tst_DateFieldRender)
#include "tst_datefield_render.moc"
