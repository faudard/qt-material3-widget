#include <QtTest/QtTest>
#include <QSignalSpy>
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"
#include "qtmaterial/theme/qtmaterialthemecontextdefaults.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"

class ThemeRuntimeTest : public QObject {
    Q_OBJECT
private slots:
    void defaultProviderMatchesManager();
    void equalContextUpdateIsNoOp();
    void contextRevisionIncrementsExactlyOnce();
    void managerRevisionDelegatesToDefaultContext();
    void independentContextsRemainIsolated();
};

void ThemeRuntimeTest::defaultProviderMatchesManager()
{
    QCOMPARE(QtMaterial::ThemeContextDefaults::defaultContext(),
             QtMaterial::ThemeManager::instance().defaultContext());
}
void ThemeRuntimeTest::equalContextUpdateIsNoOp()
{
    QtMaterial::ThemeBuilder b;
    const auto t = b.buildLightFromSeed(QColor(QStringLiteral("#6750A4")));
    QtMaterial::ThemeContext c(t);
    QSignalSpy s1(&c, &QtMaterial::ThemeContext::themeChanged);
    QSignalSpy s2(&c, &QtMaterial::ThemeContext::revisionChanged);
    QVERIFY(!c.setTheme(t));
    QCOMPARE(c.revision(), quint64(0));
    QCOMPARE(s1.count(), 0);
    QCOMPARE(s2.count(), 0);
}
void ThemeRuntimeTest::contextRevisionIncrementsExactlyOnce()
{
    QtMaterial::ThemeBuilder b;
    const auto a = b.buildLightFromSeed(QColor(QStringLiteral("#6750A4")));
    const auto z = b.buildDarkFromSeed(QColor(QStringLiteral("#6750A4")));
    QtMaterial::ThemeContext c(a);
    QVERIFY(c.setTheme(z));
    QCOMPARE(c.revision(), quint64(1));
    QVERIFY(!c.setTheme(z));
    QCOMPARE(c.revision(), quint64(1));
}
void ThemeRuntimeTest::managerRevisionDelegatesToDefaultContext()
{
    auto& m = QtMaterial::ThemeManager::instance();
    QCOMPARE(m.revision(), m.defaultContext()->revision());
}
void ThemeRuntimeTest::independentContextsRemainIsolated()
{
    QtMaterial::ThemeBuilder builder;
    const auto initial =
        builder.buildLightFromSeed(QColor(QStringLiteral("#6750A4")));
    const auto changed =
        builder.buildDarkFromSeed(QColor(QStringLiteral("#00639B")));

    QtMaterial::ThemeContext first(initial);
    QtMaterial::ThemeContext second(initial);
    QSignalSpy firstSpy(&first, &QtMaterial::ThemeContext::themeChanged);
    QSignalSpy secondSpy(&second, &QtMaterial::ThemeContext::themeChanged);

    QVERIFY(first.setTheme(changed));
    QCOMPARE(first.revision(), quint64(1));
    QCOMPARE(second.revision(), quint64(0));
    QVERIFY(first.theme() == changed);
    QVERIFY(second.theme() == initial);
    QCOMPARE(firstSpy.count(), 1);
    QCOMPARE(secondSpy.count(), 0);
}

QTEST_MAIN(ThemeRuntimeTest)
#include "tst_themeruntime.moc"
