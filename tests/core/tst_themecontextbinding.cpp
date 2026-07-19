#include <QtTest>

#include "qtmaterial/core/qtmaterialthemecontextbinding.h"
#include "qtmaterial/core/qtmaterialwidget.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemecontexthost.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"

using namespace QtMaterial;

class BindingHost final : public QWidget, public ThemeContextHost
{
    Q_OBJECT
    Q_INTERFACES(QtMaterial::ThemeContextHost)

public:
    explicit BindingHost(QWidget* parent = nullptr)
        : QWidget(parent)
        , binding(new QtMaterialThemeContextBinding(this, this))
    {
    }

    void setThemeContext(ThemeContext* context)
    {
        binding->setThemeContext(context);
    }

    ThemeContext* themeContext() const noexcept override
    {
        return binding->themeContext();
    }

    ThemeContext* effectiveThemeContext() const noexcept override
    {
        return binding->effectiveThemeContext();
    }

    QtMaterialThemeContextBinding* binding;
};

class tst_ThemeContextBinding : public QObject
{
    Q_OBJECT

private slots:
    void fallsBackToGlobalContext();
    void inheritsNearestHostContext();
    void followsParentContextReplacement();
    void explicitContextWins();
};

void tst_ThemeContextBinding::fallsBackToGlobalContext()
{
    BindingHost host;
    QCOMPARE(
        host.effectiveThemeContext(),
        ThemeManager::instance().defaultContext());
}

void tst_ThemeContextBinding::inheritsNearestHostContext()
{
    const Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));
    ThemeContext context(theme);

    QtMaterialWidget parent;
    parent.setThemeContext(&context);
    BindingHost child(&parent);

    QCOMPARE(child.themeContext(), nullptr);
    QCOMPARE(child.effectiveThemeContext(), &context);
}

void tst_ThemeContextBinding::followsParentContextReplacement()
{
    ThemeContext first(
        ThemeBuilder().buildLightFromSeed(
            QColor(QStringLiteral("#6750A4"))));
    ThemeContext second(
        ThemeBuilder().buildDarkFromSeed(
            QColor(QStringLiteral("#006874"))));

    QtMaterialWidget parent;
    parent.setThemeContext(&first);
    BindingHost child(&parent);

    QSignalSpy spy(
        child.binding,
        &QtMaterialThemeContextBinding::effectiveThemeContextChanged);

    parent.setThemeContext(&second);

    QCOMPARE(child.effectiveThemeContext(), &second);
    QCOMPARE(spy.count(), 1);
}

void tst_ThemeContextBinding::explicitContextWins()
{
    ThemeContext parentContext(
        ThemeBuilder().buildLightFromSeed(
            QColor(QStringLiteral("#6750A4"))));
    ThemeContext explicitContext(
        ThemeBuilder().buildDarkFromSeed(
            QColor(QStringLiteral("#006874"))));

    QtMaterialWidget parent;
    parent.setThemeContext(&parentContext);
    BindingHost child(&parent);
    child.setThemeContext(&explicitContext);

    QCOMPARE(child.themeContext(), &explicitContext);
    QCOMPARE(child.effectiveThemeContext(), &explicitContext);
}

QTEST_MAIN(tst_ThemeContextBinding)
#include "tst_themecontextbinding.moc"
