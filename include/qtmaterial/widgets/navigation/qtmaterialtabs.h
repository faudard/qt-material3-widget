#pragma once

#include <memory>

#include <functional>
#include <utility>

#include <QPointer>
#include <QString>
#include <QTabBar>
#include <QTabWidget>
#include <QUrl>
#include <QVector>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/widgets/navigation/model/qtmaterialnavigationmodel.h"
#include "qtmaterial/specs/qtmaterialtabsspec.h"

QT_BEGIN_NAMESPACE
class QEvent;
class QStackedWidget;
QT_END_NAMESPACE

namespace QtMaterial {

class ThemeContext;
class Theme;

class QtMaterialTabsPrivate;
class QtMaterialNavigationController;
class QtMaterialTabsBar;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialRoute
{
    Q_GADGET
    Q_PROPERTY(QString path READ path)

public:
    QtMaterialRoute() = default;

    explicit QtMaterialRoute(QString path)
        : m_path(normalizedPath(std::move(path)))
    {
    }

    static QString normalizedPath(QString path)
    {
        path = path.trimmed();
        path.replace(
            QLatin1Char('\\'),
            QLatin1Char('/'));

        if (path.isEmpty()) {
            return QString();
        }

        QString normalized;
        normalized.reserve(path.size() + 1);

        bool previousWasSlash = false;
        for (const QChar character : path) {
            const bool isSlash =
                character == QLatin1Char('/');

            if (isSlash) {
                if (!previousWasSlash) {
                    normalized.append(
                        QLatin1Char('/'));
                }
            } else {
                normalized.append(character);
            }

            previousWasSlash = isSlash;
        }

        while (
            normalized.size() > 1
            && normalized.endsWith(
                QLatin1Char('/'))) {
            normalized.chop(1);
        }

        if (
            !normalized.startsWith(
                QLatin1Char('/'))) {
            normalized.prepend(
                QLatin1Char('/'));
        }

        return normalized;
    }

    QString path() const
    {
        return m_path;
    }

    QString toString() const
    {
        return m_path;
    }

    bool isValid() const
    {
        return !m_path.isEmpty();
    }

    bool operator==(
        const QtMaterialRoute& other) const
    {
        return m_path == other.m_path;
    }

    bool operator!=(
        const QtMaterialRoute& other) const
    {
        return !(*this == other);
    }

private:
    QString m_path;
};

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialTabs : public QTabWidget {
    Q_OBJECT
    Q_PROPERTY(TabsVariant variant READ variant WRITE setVariant NOTIFY variantChanged)
    Q_PROPERTY(TabsDensity density READ density WRITE setDensity NOTIFY densityChanged)
    Q_PROPERTY(TabsAlignment alignment READ alignment WRITE setAlignment NOTIFY alignmentChanged)
    Q_PROPERTY(bool scrollable READ isScrollable WRITE setScrollable NOTIFY scrollableChanged)
    Q_PROPERTY(int indicatorHeight READ indicatorHeight WRITE setIndicatorHeight NOTIFY indicatorHeightChanged)
    Q_PROPERTY(TabsOverflowMode overflowMode READ overflowMode WRITE setOverflowMode NOTIFY overflowModeChanged)
    Q_PROPERTY(bool useGlobalTheme READ usesGlobalTheme WRITE setUseGlobalTheme NOTIFY useGlobalThemeChanged)
    Q_PROPERTY(QtMaterial::ThemeContext* themeContext READ themeContext WRITE setThemeContext NOTIFY themeContextChanged)
    Q_PROPERTY(bool wrapNavigation READ wrapNavigation WRITE setWrapNavigation NOTIFY wrapNavigationChanged)
    Q_PROPERTY(QString accessibilitySummary READ accessibilitySummary NOTIFY accessibilitySummaryChanged)
    Q_PROPERTY(bool lazyLoading READ lazyLoading WRITE setLazyLoading NOTIFY lazyLoadingChanged)
    Q_PROPERTY(QtMaterial::QtMaterialNavigationModel* navigationModel READ navigationModel WRITE setNavigationModel NOTIFY navigationModelChanged)

public:
    explicit QtMaterialTabs(QWidget* parent = nullptr);
    explicit QtMaterialTabs(const TabsSpec& spec, QWidget* parent = nullptr);
    ~QtMaterialTabs() override;

    TabsSpec spec() const;
    TabsSpec authoredSpec() const;
    TabsSpec resolvedSpec() const;
    void setSpec(const TabsSpec& spec);

    TabsVariant variant() const;
    void setVariant(TabsVariant variant);

    TabsDensity density() const;
    void setDensity(TabsDensity density);

    TabsAlignment alignment() const;
    void setAlignment(TabsAlignment alignment);

    bool isScrollable() const;
    void setScrollable(bool scrollable);

    int indicatorHeight() const;
    void setIndicatorHeight(int height);

    TabsOverflowMode overflowMode() const;
    void setOverflowMode(TabsOverflowMode mode);

    bool usesGlobalTheme() const;
    void setUseGlobalTheme(bool enabled);
    void refreshTheme();
    void setThemeContext(QtMaterial::ThemeContext* context);
    QtMaterial::ThemeContext* themeContext() const noexcept;
    QtMaterial::ThemeContext* effectiveThemeContext() const noexcept;

    bool wrapNavigation() const;
    void setWrapNavigation(bool enabled);

    QString currentTabAccessibleText() const;
    QString accessibilitySummary() const;

    void setBadge(int index, const QString& text);
    QString badge(int index) const;
    void setBadgeVisible(int index, bool visible);
    bool isBadgeVisible(int index) const;
    void clearBadge(int index);

    void setTabId(int index, const QString& id);
    QString tabId(int index) const;
    int indexOfTabId(const QString& id) const;

    void setTabTestId(int index, const QString& testId);
    QString tabTestId(int index) const;
    int indexOfTabTestId(const QString& testId) const;

    void setRoute(int index, const QtMaterialRoute& route);
    void setRoute(int index, const QString& routePath);
    QtMaterialRoute route(int index) const;
    int indexOfRoute(const QtMaterialRoute& route) const;
    int indexOfRoute(const QString& routePath) const;

    bool navigateTo(const QtMaterialRoute& route);
    bool navigateTo(const QString& routePath);
    bool navigateToUrl(const QUrl& url);

    bool lazyLoading() const;
    void setLazyLoading(bool enabled);
    void setTabFactory(int index, std::function<QWidget*()> factory);
    void setTabFactory(const QtMaterialRoute& route, std::function<QWidget*()> factory);
    bool isTabLoaded(int index) const;
    QWidget* ensureTabLoaded(int index);

    void bindTo(QStackedWidget* stack);
    QtMaterialNavigationModel* navigationModel() const;
    void setNavigationModel(QtMaterialNavigationModel* model);
    void bindToController(QtMaterialNavigationController* controller);
    void unbindController(QtMaterialNavigationController* controller);
    void unbindAll();
    void unbindStackedWidget();
    QStackedWidget* boundStackedWidget() const;
    QVector<QtMaterialNavigationController*> boundControllers() const;

signals:
    void specChanged();
    void variantChanged(QtMaterial::TabsVariant variant);
    void densityChanged(QtMaterial::TabsDensity density);
    void alignmentChanged(QtMaterial::TabsAlignment alignment);
    void scrollableChanged(bool scrollable);
    void indicatorHeightChanged(int height);
    void overflowModeChanged(QtMaterial::TabsOverflowMode mode);
    void useGlobalThemeChanged(bool enabled);
    void themeContextChanged(QtMaterial::ThemeContext* context);
    void effectiveThemeContextChanged(QtMaterial::ThemeContext* context);
    void wrapNavigationChanged(bool enabled);
    void accessibilitySummaryChanged(const QString& summary);
    void lazyLoadingChanged(bool enabled);
    void badgeChanged(int index);
    void tabIdChanged(int index, const QString& id);
    void tabTestIdChanged(int index, const QString& testId);
    void routeChanged(int index, const QtMaterial::QtMaterialRoute& route);
    void currentRouteChanged(const QtMaterial::QtMaterialRoute& route);
    void tabLoaded(int index, QWidget* widget);
    void stackBindingChanged(QStackedWidget* stack);
    void controllerBindingChanged(QtMaterialNavigationController* controller, bool bound);
    void navigationModelChanged(QtMaterial::QtMaterialNavigationModel* model);

protected:
    void changeEvent(QEvent* event) override;
    bool event(QEvent* event) override;
    void tabInserted(int index) override;
    void tabRemoved(int index) override;

private slots:
    void onCurrentTabChanged(int index);

private:
    friend class QtMaterialTabsPrivate;

 struct TabDescriptor;

 TabDescriptor* descriptor(int index);

 const TabDescriptor* descriptor(int index) const;

 void resolveSpecFromTheme();
    bool refreshThemeContextConnection();
    void handleThemeChanged(const QtMaterial::Theme& theme);
    void handleInheritedThemeContextChanged(QtMaterial::ThemeContext* context);
    void handleThemeContextDestroyed(bool explicitContext);

 void applyResolvedSpec();

 void ensureDescriptorCount(int count);
 void updateAutomationMetadata(int index);

 void updateAllAutomationMetadata();

 void syncAccessibilityState();

 void syncControllersFromCurrentIndex(int index);

 void syncNavigationModelFromTabs();

 void syncNavigationModelSelectionFromCurrentTab();
 void emitCurrentRouteIfChanged();

 void syncCurrentIndexFromController(int index);

 static QString normalizeRoutePath(const QString& routePath);

 static QString routePathFromUrl(const QUrl& url);

 QtMaterialTabsBar* materialTabBar() const;

 std::unique_ptr<QtMaterialTabsPrivate> d_ptr;

};

} // namespace QtMaterial

Q_DECLARE_METATYPE(QtMaterial::QtMaterialRoute)
