#include <QtTest/QtTest>

#include <QSignalSpy>

#include "qtmaterial/core/qtmaterialabstractbutton.h"
#include "qtmaterial/core/qtmaterialwidget.h"
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

class ProbeButton final : public QtMaterialAbstractButton
{
public:
    using QtMaterialAbstractButton::QtMaterialAbstractButton;

    QColor sourceColor() const
    {
        return theme().options().sourceColor;
    }

    int themeChangeCount() const noexcept
    {
        return m_themeChangeCount;
    }

    int invalidationCount() const noexcept
    {
        return m_invalidationCount;
    }

    void resetCounters() noexcept
    {
        m_themeChangeCount = 0;
        m_invalidationCount = 0;
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
    }

    void themeChangedEvent(const Theme& theme) override
    {
        ++m_themeChangeCount;
        QtMaterialAbstractButton::themeChangedEvent(theme);
    }

    void invalidateResolvedSpec() override
    {
        ++m_invalidationCount;
    }

private:
    int m_themeChangeCount = 0;
    int m_invalidationCount = 0;
};

} // namespace

class tst_AbstractButtonThemeContext final : public QObject
{
    Q_OBJECT

private slots:
    void fallsBackToGlobalContext();
    void explicitContextOverridesGlobalContext();
    void inheritsParentContext();
    void replacingParentContextPropagates();
    void explicitChildContextStopsInheritance();
    void themeRevisionInvalidatesResolvedSpec();
    void themeContextHostInterfaceIsDiscoverable();
};

void tst_AbstractButtonThemeContext::fallsBackToGlobalContext()
{
    ProbeButton button;

    QCOMPARE(
        button.effectiveThemeContext(),
        ThemeManager::instance().defaultContext());
}

void tst_AbstractButtonThemeContext::explicitContextOverridesGlobalContext()
{
    ThemeContext context(makeTheme(QStringLiteral("#6750A4")));
    ProbeButton button;
    QSignalSpy contextSpy(
        &button,
        &QtMaterialAbstractButton::effectiveThemeContextChanged);

    button.setThemeContext(&context);

    QCOMPARE(button.themeContext(), &context);
    QCOMPARE(button.effectiveThemeContext(), &context);
    QCOMPARE(button.sourceColor(), QColor("#6750A4"));
    QCOMPARE(contextSpy.count(), 1);
    QVERIFY(button.themeChangeCount() >= 1);
    QVERIFY(button.invalidationCount() >= 1);
}

void tst_AbstractButtonThemeContext::inheritsParentContext()
{
    ThemeContext context(makeTheme(QStringLiteral("#006A6A")));
    QtMaterialWidget parent;
    ProbeButton child(&parent);

    parent.setThemeContext(&context);

    QCOMPARE(child.themeContext(), nullptr);
    QCOMPARE(child.effectiveThemeContext(), &context);
    QCOMPARE(child.sourceColor(), QColor("#006A6A"));
}

void tst_AbstractButtonThemeContext::replacingParentContextPropagates()
{
    ThemeContext first(makeTheme(QStringLiteral("#6750A4")));
    ThemeContext second(makeTheme(QStringLiteral("#7D5260")));
    QtMaterialWidget parent;
    ProbeButton child(&parent);

    parent.setThemeContext(&first);
    child.resetCounters();

    QSignalSpy contextSpy(
        &child,
        &QtMaterialAbstractButton::effectiveThemeContextChanged);

    parent.setThemeContext(&second);

    QCOMPARE(child.effectiveThemeContext(), &second);
    QCOMPARE(child.sourceColor(), QColor("#7D5260"));
    QCOMPARE(contextSpy.count(), 1);
    QVERIFY(child.themeChangeCount() >= 1);
    QVERIFY(child.invalidationCount() >= 1);
}

void tst_AbstractButtonThemeContext::explicitChildContextStopsInheritance()
{
    ThemeContext parentContext(makeTheme(QStringLiteral("#006A6A")));
    ThemeContext childContext(makeTheme(QStringLiteral("#B3261E")));
    ThemeContext replacementParentContext(makeTheme(QStringLiteral("#6750A4")));
    QtMaterialWidget parent;
    ProbeButton child(&parent);

    parent.setThemeContext(&parentContext);
    child.setThemeContext(&childContext);
    parent.setThemeContext(&replacementParentContext);

    QCOMPARE(child.themeContext(), &childContext);
    QCOMPARE(child.effectiveThemeContext(), &childContext);
    QCOMPARE(child.sourceColor(), QColor("#B3261E"));

    child.setThemeContext(nullptr);

    QCOMPARE(child.themeContext(), nullptr);
    QCOMPARE(child.effectiveThemeContext(), &replacementParentContext);
    QCOMPARE(child.sourceColor(), QColor("#6750A4"));
}

void tst_AbstractButtonThemeContext::themeRevisionInvalidatesResolvedSpec()
{
    ThemeContext context(makeTheme(QStringLiteral("#6750A4")));
    ProbeButton button;
    button.setThemeContext(&context);
    button.resetCounters();

    QVERIFY(context.setTheme(makeTheme(QStringLiteral("#006A6A"))));

    QCOMPARE(button.sourceColor(), QColor("#006A6A"));
    QCOMPARE(button.themeChangeCount(), 1);
    QCOMPARE(button.invalidationCount(), 1);
}

void tst_AbstractButtonThemeContext::themeContextHostInterfaceIsDiscoverable()
{
    ProbeButton button;
    auto* host = qobject_cast<ThemeContextHost*>(&button);

    QVERIFY(host != nullptr);
    QCOMPARE(host->effectiveThemeContext(), button.effectiveThemeContext());
}

QTEST_MAIN(tst_AbstractButtonThemeContext)

#include "tst_abstractbutton_themecontext.moc"
