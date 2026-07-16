#include <QColor>
#include <QtTest>

#include "qtmaterial/core/qtmaterialwidget.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"

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
    void explicitContextOverridesGlobalContext();
    void childInheritsParentContext();
    void replacingParentContextPropagates();
    void childExplicitContextStopsInheritance();
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

QTEST_MAIN(tst_WidgetThemeContext)

#include "tst_widget_themecontext.moc"
