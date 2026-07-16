#include <QColor>
#include <QSignalSpy>
#include <QtTest>

#include "qtmaterial/theme/qtmaterialthemecontext.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"

using namespace QtMaterial;

namespace {

Theme makeTheme(const QString& primary)
{
    Theme theme;
    ThemeOptions options;
    options.sourceColor = QColor(primary);
    theme.setOptions(options);
    theme.colorScheme().setColor(ColorRole::Primary, QColor(primary));
    return theme;
}

} // namespace

class tst_ThemeContext : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void identicalThemeIsIgnored();
    void changedThemeAdvancesRevision();
};

void tst_ThemeContext::initTestCase()
{
    qRegisterMetaType<QtMaterial::Theme>("QtMaterial::Theme");
}

void tst_ThemeContext::identicalThemeIsIgnored()
{
    const Theme initial = makeTheme(QStringLiteral("#146C2E"));
    ThemeContext context(initial);

    QSignalSpy themeSpy(&context, &ThemeContext::themeChanged);
    QSignalSpy revisionSpy(&context, &ThemeContext::revisionChanged);

    QVERIFY(!context.setTheme(initial));
    QCOMPARE(context.revision(), quint64(0));
    QCOMPARE(themeSpy.count(), 0);
    QCOMPARE(revisionSpy.count(), 0);
}

void tst_ThemeContext::changedThemeAdvancesRevision()
{
    ThemeContext context(makeTheme(QStringLiteral("#146C2E")));

    QSignalSpy themeSpy(&context, &ThemeContext::themeChanged);
    QSignalSpy revisionSpy(&context, &ThemeContext::revisionChanged);

    QVERIFY(context.setTheme(makeTheme(QStringLiteral("#6750A4"))));
    QCOMPARE(context.revision(), quint64(1));
    QCOMPARE(context.theme().options().sourceColor,
             QColor(QStringLiteral("#6750A4")));
    QCOMPARE(themeSpy.count(), 1);
    QCOMPARE(revisionSpy.count(), 1);
}

QTEST_MAIN(tst_ThemeContext)

#include "tst_themecontext.moc"
