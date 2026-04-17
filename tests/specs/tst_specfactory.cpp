#include <QtTest/QtTest>
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
class tst_SpecFactory : public QObject
{
    Q_OBJECT
private slots:
    void createsButtonSpec();
    void createsDialogSpec();
};
void tst_SpecFactory::createsButtonSpec()
{
    QtMaterial::ThemeBuilder builder;
    QtMaterial::SpecFactory factory;
    const QtMaterial::Theme theme = builder.buildLightFromSeed(QColor("#6750A4"));
    const QtMaterial::ButtonSpec spec = factory.filledButtonSpec(theme);
    QVERIFY(spec.containerColor.isValid());
    QVERIFY(spec.labelColor.isValid());
    QCOMPARE(spec.containerHeight, 40);
}
void tst_SpecFactory::createsDialogSpec()
{
    QtMaterial::ThemeBuilder builder;
    QtMaterial::SpecFactory factory;
    const QtMaterial::Theme theme = builder.buildDarkFromSeed(QColor("#6750A4"));
    const QtMaterial::DialogSpec spec = factory.dialogSpec(theme);
    QVERIFY(spec.containerColor.isValid());
    QVERIFY(spec.scrimColor.isValid());
    QCOMPARE(spec.maxWidth, 560);
}
QTEST_MAIN(tst_SpecFactory)
#include "tst_specfactory.moc"
