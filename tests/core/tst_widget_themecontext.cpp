#include <QColor>
#include <QtTest>

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

class ProbeWidget final : public QtMaterialWidget
{
public:
    using QtMaterialWidget::QtMaterialWidget;

    QColor sourceColor() const
    {
        return theme().options().sourceColor;
    }

    int themeChangeCount() const noexcept
    {
        return m_themeChangeCount;
    }

protected:
    void themeChangedEvent(const Theme& changedTheme) override
    {
        ++m_themeChangeCount;
        QtMaterialWidget::themeChangedEvent(changedTheme);
    }

private:
    int m_themeChangeCount = 0;
};

} // namespace

class tst_WidgetThemeContext : public QObject
{
    Q_OBJECT

private slots:
    
    void fallsBackToGlobalContext();void explicitContextOverridesGlobalContext();
    void childInheritsParentContext();
    void replacingParentContextPropagates();
    void childExplicitContextStopsInheritance();
    void reparentingRebindsEffectiveContext();
    void destroyedExplicitContextFallsBackToParent();
    void themeContextHostInterfaceIsDiscoverable();
};

void tst_WidgetThemeContext::explicitContextOverridesGlobalContext()
{
    ThemeContext context(makeTheme(QStringLiteral("#146C2E")));
    ProbeWidget widget;

    widget.setThemeContext(&context);

    QCOMPARE(widget.themeContext(), &context);
    QCOMPARE(widget.effectiveThemeContext(), &context);
    QCOMPARE(widget.sourceColor(), QColor(QStringLiteral("#146C2E")));
}

void tst_WidgetThemeContext::childInheritsParentContext()
{
    ThemeContext context(makeTheme(QStringLiteral("#146C2E")));
    ProbeWidget parent;
    ProbeWidget child(&parent);

    parent.setThemeContext(&context);

    QCOMPARE(child.themeContext(), nullptr);
    QCOMPARE(child.effectiveThemeContext(), &context);
    QCOMPARE(child.sourceColor(), QColor(QStringLiteral("#146C2E")));
}

void tst_WidgetThemeContext::replacingParentContextPropagates()
{
    ThemeContext first(makeTheme(QStringLiteral("#146C2E")));
    ThemeContext second(makeTheme(QStringLiteral("#6750A4")));
    ProbeWidget parent;
    ProbeWidget child(&parent);

    parent.setThemeContext(&first);
    const int previousChangeCount = child.themeChangeCount();

    parent.setThemeContext(&second);

    QCOMPARE(child.effectiveThemeContext(), &second);
    QCOMPARE(child.sourceColor(), QColor(QStringLiteral("#6750A4")));
    QVERIFY(child.themeChangeCount() > previousChangeCount);
}

void tst_WidgetThemeContext::childExplicitContextStopsInheritance()
{
    ThemeContext parentContext(makeTheme(QStringLiteral("#146C2E")));
    ThemeContext childContext(makeTheme(QStringLiteral("#6750A4")));
    ThemeContext replacementParentContext(makeTheme(QStringLiteral("#B3261E")));

    ProbeWidget parent;
    ProbeWidget child(&parent);

    parent.setThemeContext(&parentContext);
    child.setThemeContext(&childContext);
    parent.setThemeContext(&replacementParentContext);

    QCOMPARE(child.effectiveThemeContext(), &childContext);
    QCOMPARE(child.sourceColor(), QColor(QStringLiteral("#6750A4")));

    child.setThemeContext(nullptr);
    QCOMPARE(child.effectiveThemeContext(), &replacementParentContext);
    QCOMPARE(child.sourceColor(), QColor(QStringLiteral("#B3261E")));
}


void tst_WidgetThemeContext::fallsBackToGlobalContext()
{
    ProbeWidget widget;

    QCOMPARE(
        widget.effectiveThemeContext(),
        ThemeManager::instance().defaultContext());
}

void tst_WidgetThemeContext::reparentingRebindsEffectiveContext()
{
    ThemeContext firstContext(makeTheme(QStringLiteral("#146C2E")));
    ThemeContext secondContext(makeTheme(QStringLiteral("#6750A4")));
    ProbeWidget firstParent;
    ProbeWidget secondParent;
    ProbeWidget child(&firstParent);

    firstParent.setThemeContext(&firstContext);
    secondParent.setThemeContext(&secondContext);
    QCOMPARE(child.effectiveThemeContext(), &firstContext);

    child.setParent(&secondParent);

    QCOMPARE(child.effectiveThemeContext(), &secondContext);
    QCOMPARE(child.sourceColor(), QColor(QStringLiteral("#6750A4")));
}

void tst_WidgetThemeContext::destroyedExplicitContextFallsBackToParent()
{
    ThemeContext parentContext(makeTheme(QStringLiteral("#146C2E")));
    ProbeWidget parent;
    ProbeWidget child(&parent);
    parent.setThemeContext(&parentContext);

    auto* explicitContext = new ThemeContext(
        makeTheme(QStringLiteral("#6750A4")));
    child.setThemeContext(explicitContext);
    QCOMPARE(child.effectiveThemeContext(), explicitContext);

    delete explicitContext;

    QCOMPARE(child.themeContext(), nullptr);
    QCOMPARE(child.effectiveThemeContext(), &parentContext);
    QCOMPARE(child.sourceColor(), QColor(QStringLiteral("#146C2E")));
}

void tst_WidgetThemeContext::themeContextHostInterfaceIsDiscoverable()
{
    ProbeWidget widget;
    auto* host = qobject_cast<ThemeContextHost*>(&widget);

    QVERIFY(host != nullptr);
    QCOMPARE(host->effectiveThemeContext(), widget.effectiveThemeContext());
}

QTEST_MAIN(tst_WidgetThemeContext)

#include "tst_widget_themecontext.moc"
