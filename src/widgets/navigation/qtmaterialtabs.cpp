#include "qtmaterial/widgets/navigation/qtmaterialtabs.h"
#include "qtmaterial/core/qtmaterialnavigationmodel.h"

#include <QAction>
#include <QApplication>
#include <QEvent>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QPointer>
#include <QSignalBlocker>
#include <QStackedWidget>
#include <QStyleOptionTab>
#include <QTabBar>
#include <QToolButton>
#include <QVariantAnimation>
#include <QVBoxLayout>

#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/widgets/navigation/qtmaterialnavigationcontroller.h"
#include <memory>

namespace QtMaterial {

struct QtMaterialTabs::TabDescriptor {
 QString id;
 QString testId;
 QtMaterialRoute route;
 std::function<QWidget*()> factory;
 bool loaded = true;
 QString badgeText;
 bool badgeVisible = false;
};

class QtMaterialTabsPrivate {
public:
 TabsSpec authoredSpec;
 TabsSpec resolvedSpec;
 QPointer<ThemeContext> themeContext;
 QPointer<ThemeContext> effectiveThemeContext;
 QMetaObject::Connection themeChangedConnection;
 QMetaObject::Connection themeDestroyedConnection;
 QMetaObject::Connection ancestorContextConnection;
 QVector<QtMaterialTabs::TabDescriptor> descriptors;
 QVector<QPointer<QtMaterialNavigationController>> boundControllers;
 QPointer<QStackedWidget> boundStack;
 QPointer<QtMaterialNavigationModel> navigationModel;
 bool lazyLoading = false;
 bool syncingExternal = false;
 bool syncingNavigationModel = false;
 QString lastAccessibilitySummary;
 QString lastEmittedCurrentRoutePath;
};

namespace {

QColor withAlpha(QColor color, qreal opacity)
{
    color.setAlphaF(qBound<qreal>(0.0, opacity, 1.0));
    return color;
}

int resolvedHeight(const TabsSpec& spec)
{
    return spec.density == TabsDensity::Compact ? qMin(spec.containerHeight, 40) : spec.containerHeight;
}

bool usesOverflowMenu(TabsOverflowMode mode)
{
    return mode == TabsOverflowMode::Menu || mode == TabsOverflowMode::ScrollButtonsAndMenu;
}

bool usesScrollButtons(TabsOverflowMode mode)
{
    return mode == TabsOverflowMode::ScrollButtons || mode == TabsOverflowMode::ScrollButtonsAndMenu;
}


QString canonicalTabsRoutePath(
    const QString& routePath)
{
    return QtMaterialRoute::normalizedPath(
        routePath);
}

QString qtm3TabsNavigationModelRoutePath(
    const QString& routePath)
{
    return canonicalTabsRoutePath(
        routePath);
}

QString qtm3NavigationModelRoutePath(
    const QString& routePath)
{
    return canonicalTabsRoutePath(
        routePath);
}

} // namespace

class QtMaterialTabsBar final : public QTabBar {
    Q_OBJECT

public:
    explicit QtMaterialTabsBar(QWidget* parent = nullptr)
        : QTabBar(parent)
        , m_indicatorAnimation(new QVariantAnimation(this))
        , m_overflowButton(new QToolButton(this))
        , m_overflowMenu(new QMenu(this))
    {
        setDrawBase(false);
        setMovable(false);
        setTabsClosable(false);
        setUsesScrollButtons(true);
        setElideMode(Qt::ElideRight);
        setMouseTracking(true);
        setFocusPolicy(Qt::StrongFocus);

        m_overflowButton->setText(QStringLiteral("…"));
        m_overflowButton->setAutoRaise(true);
        m_overflowButton->setFocusPolicy(Qt::NoFocus);
        m_overflowButton->setMenu(m_overflowMenu);
        m_overflowButton->setPopupMode(QToolButton::InstantPopup);
        m_overflowButton->hide();

        m_indicatorAnimation->setDuration(180);
        connect(this, &QTabBar::currentChanged, this, &QtMaterialTabsBar::animateIndicatorToCurrentTab);
        connect(m_overflowMenu, &QMenu::triggered, this, [this](QAction* action) {
            bool ok = false;
            const int index = action->data().toInt(&ok);
            if (ok) {
                setCurrentIndex(index);
            }
        });
    }

    void setResolvedSpec(const TabsSpec& spec)
    {
        m_spec = spec;
        m_indicatorAnimation->setDuration(qMax(0, spec.animationDuration));
        setUsesScrollButtons(::QtMaterial::usesScrollButtons(spec.overflowMode) && spec.scrollable);
        setExpanding(spec.alignment == TabsAlignment::Stretch);
        updateStyleSheet();
        rebuildOverflowMenu();
        updateOverflowButtonGeometry();
        update();
    }

    void setBadgeProvider(std::function<QPair<QString, bool>(int)> provider)
    {
        m_badgeProvider = std::move(provider);
        update();
    }

    void setWrapNavigation(bool enabled)
    {
        m_wrapNavigation = enabled;
    }

protected:
    void resizeEvent(QResizeEvent* event) override
    {
        QTabBar::resizeEvent(event);
        updateOverflowButtonGeometry();
    }

    void tabLayoutChange() override
    {
        QTabBar::tabLayoutChange();
        updateOverflowButtonGeometry();
        rebuildOverflowMenu();
        if (currentIndex() >= 0) {
            m_indicatorRect = indicatorRectForIndex(currentIndex());
        }
    }

    void leaveEvent(QEvent* event) override
    {
        m_hoveredIndex = -1;
        QTabBar::leaveEvent(event);
        update();
    }

    void mouseMoveEvent(QMouseEvent* event) override
    {
        m_hoveredIndex = tabAt(event->pos());
        QTabBar::mouseMoveEvent(event);
        update();
    }

    void mousePressEvent(QMouseEvent* event) override
    {
        if (event->button() == Qt::LeftButton) {
            m_pressedIndex = tabAt(event->pos());
        }
        QTabBar::mousePressEvent(event);
        update();
    }

    void mouseReleaseEvent(QMouseEvent* event) override
    {
        m_pressedIndex = -1;
        QTabBar::mouseReleaseEvent(event);
        update();
    }

    void keyPressEvent(QKeyEvent* event) override
    {
        const bool backward = event->matches(QKeySequence::Back) ||
            (event->modifiers().testFlag(Qt::ControlModifier) && event->key() == Qt::Key_Tab && event->modifiers().testFlag(Qt::ShiftModifier));
        const bool forward = event->matches(QKeySequence::Forward) ||
            (event->modifiers().testFlag(Qt::ControlModifier) && event->key() == Qt::Key_Tab && !event->modifiers().testFlag(Qt::ShiftModifier));

        if (event->key() == Qt::Key_Home) {
            moveToEnabledIndex(0, +1, false);
            event->accept();
            return;
        }
        if (event->key() == Qt::Key_End) {
            moveToEnabledIndex(count() - 1, -1, false);
            event->accept();
            return;
        }
        if (event->key() == Qt::Key_Left || event->key() == Qt::Key_Up || backward) {
            moveRelative(-1);
            event->accept();
            return;
        }
        if (event->key() == Qt::Key_Right || event->key() == Qt::Key_Down || forward) {
            moveRelative(+1);
            event->accept();
            return;
        }
        QTabBar::keyPressEvent(event);
    }

    void paintEvent(QPaintEvent* event) override
    {
        QTabBar::paintEvent(event);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        const auto drawStateLayer = [&](int index, const QColor& color, qreal opacity) {
            if (index < 0 || index >= count()) {
                return;
            }
            QRect rect = tabRect(index).adjusted(4, 4, -4, -4);
            if (!rect.isValid()) {
                return;
            }
            painter.setPen(Qt::NoPen);
            painter.setBrush(withAlpha(color, opacity));
            painter.drawRoundedRect(rect, 12.0, 12.0);
        };

        if (m_hoveredIndex >= 0 && m_hoveredIndex != currentIndex()) {
            drawStateLayer(m_hoveredIndex,
                           m_spec.hoverStateLayerColor.isValid() ? m_spec.hoverStateLayerColor : m_spec.stateLayerColor,
                           qMax<qreal>(0.0, m_spec.hoverOpacity));
        }
        if (m_pressedIndex >= 0) {
            drawStateLayer(m_pressedIndex,
                           m_spec.pressedStateLayerColor.isValid() ? m_spec.pressedStateLayerColor : m_spec.stateLayerColor,
                           qMax<qreal>(0.0, m_spec.pressedOpacity));
        }
        if (hasFocus() && currentIndex() >= 0) {
            QRect focusRect = tabRect(currentIndex()).adjusted(3, 3, -3, -3);
            QPen pen(m_spec.focusRingColor, 1.5);
            painter.setPen(pen);
            painter.setBrush(Qt::NoBrush);
            painter.drawRoundedRect(focusRect, 12.0, 12.0);
            drawStateLayer(currentIndex(),
                           m_spec.focusedStateLayerColor.isValid() ? m_spec.focusedStateLayerColor : m_spec.stateLayerColor,
                           qMax<qreal>(0.0, m_spec.focusOpacity));
        }

        if (m_indicatorRect.isValid() && currentIndex() >= 0) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(m_spec.activeIndicatorColor);
            painter.drawRoundedRect(m_indicatorRect, m_spec.indicatorHeight / 2.0, m_spec.indicatorHeight / 2.0);
        }

        if (m_badgeProvider) {
            for (int i = 0; i < count(); ++i) {
                const auto badge = m_badgeProvider(i);
                if (!badge.second) {
                    continue;
                }
                const QRect rect = tabRect(i);
                const bool isDot = badge.first.isEmpty();
                const int diameter = isDot ? m_spec.badgeDotDiameter : m_spec.badgeDiameter;
                QRect badgeRect(rect.right() - diameter - 8, rect.top() + 6, diameter, diameter);
                painter.setPen(Qt::NoPen);
                painter.setBrush(m_spec.badgeColor);
                painter.drawEllipse(badgeRect);
                if (!isDot) {
                    painter.setPen(m_spec.badgeLabelColor);
                    QFont font = painter.font();
                    font.setBold(true);
                    font.setPointSizeF(qMax<qreal>(8.0, font.pointSizeF() - 1.0));
                    painter.setFont(font);
                    painter.drawText(badgeRect, Qt::AlignCenter, badge.first);
                }
            }
        }
    }

private:
    void updateStyleSheet()
    {
        const QString style = QStringLiteral(R"(
            QTabBar::tab {
                border: 0;
                background: transparent;
                min-height: %1px;
                min-width: %2px;
                max-width: %3px;
                padding: %4px %5px %6px %7px;
                color: %8;
            }
            QTabBar::tab:selected {
                color: %9;
            }
            QTabBar::tab:disabled {
                color: %10;
            }
            QToolButton {
                border: 0;
                padding: 4px;
                color: %11;
                background: transparent;
            }
            QToolButton::menu-indicator {
                image: none;
                width: 0px;
            }
        )")
            .arg(resolvedHeight(m_spec))
            .arg(m_spec.minimumTabWidth)
            .arg(m_spec.maximumTabWidth)
            .arg(m_spec.tabPadding.top())
            .arg(m_spec.tabPadding.right())
            .arg(m_spec.tabPadding.bottom())
            .arg(m_spec.tabPadding.left())
            .arg(m_spec.inactiveLabelColor.name(QColor::HexArgb))
            .arg(m_spec.activeLabelColor.name(QColor::HexArgb))
            .arg(m_spec.disabledLabelColor.name(QColor::HexArgb))
            .arg(m_spec.overflowButtonColor.name(QColor::HexArgb));
        setStyleSheet(style);
    }

    QRect indicatorRectForIndex(int index) const
    {
        if (index < 0 || index >= count()) {
            return {};
        }
        const QRect rect = tabRect(index);
        return QRect(rect.left() + 10,
                     rect.bottom() - m_spec.indicatorHeight + 1,
                     qMax(0, rect.width() - 20),
                     m_spec.indicatorHeight);
    }

    void animateIndicatorToCurrentTab(int index)
    {
        const QRect target = indicatorRectForIndex(index);
        if (!m_indicatorRect.isValid()) {
            m_indicatorRect = target;
            update();
            return;
        }

        if (target == m_indicatorRect) {
            return;
        }

        m_indicatorAnimation->stop();
        disconnect(m_indicatorAnimation, nullptr, this, nullptr);
        m_indicatorAnimation->setStartValue(m_indicatorRect);
        m_indicatorAnimation->setEndValue(target);
        connect(m_indicatorAnimation, &QVariantAnimation::valueChanged, this, [this](const QVariant& value) {
            m_indicatorRect = value.toRect();
            update();
        });
        m_indicatorAnimation->start();
    }

    bool needsOverflowButton() const
    {
        if (!usesOverflowMenu(m_spec.overflowMode)) {
            return false;
        }
        int totalWidth = 0;
        for (int i = 0; i < count(); ++i) {
            totalWidth += tabRect(i).width();
        }
        return totalWidth > width();
    }

    void rebuildOverflowMenu()
    {
        m_overflowMenu->clear();
        for (int i = 0; i < count(); ++i) {
            QAction* action = m_overflowMenu->addAction(tabIcon(i), tabText(i));
            action->setData(i);
            action->setCheckable(true);
            action->setChecked(i == currentIndex());
            action->setEnabled(isTabEnabled(i));
        }
        m_overflowButton->setVisible(needsOverflowButton());
    }

    void updateOverflowButtonGeometry()
    {
        if (!m_overflowButton->isVisible()) {
            return;
        }
        const int side = qMax(24, height() - 8);
        m_overflowButton->setGeometry(width() - side - 4, (height() - side) / 2, side, side);
    }

    void moveToEnabledIndex(int start, int step, bool allowWrap)
    {
        if (count() <= 0) {
            return;
        }
        int index = start;
        const int initial = start;
        while (index >= 0 && index < count()) {
            if (isTabEnabled(index)) {
                setCurrentIndex(index);
                return;
            }
            index += step;
        }
        if (!allowWrap || !m_wrapNavigation) {
            return;
        }
        index = step > 0 ? 0 : count() - 1;
        while (index >= 0 && index < count() && index != initial) {
            if (isTabEnabled(index)) {
                setCurrentIndex(index);
                return;
            }
            index += step;
        }
    }

    void moveRelative(int delta)
    {
        if (count() <= 0) {
            return;
        }
        int index = currentIndex();
        if (index < 0) {
            index = 0;
        }
        int next = index + delta;
        if (m_wrapNavigation) {
            if (next < 0) {
                next = count() - 1;
            }
            if (next >= count()) {
                next = 0;
            }
        }
        moveToEnabledIndex(next, delta >= 0 ? +1 : -1, true);
    }

    TabsSpec m_spec;
    QVariantAnimation* m_indicatorAnimation = nullptr;
    QRect m_indicatorRect;
    int m_hoveredIndex = -1;
    int m_pressedIndex = -1;
    bool m_wrapNavigation = true;
    QToolButton* m_overflowButton = nullptr;
    QMenu* m_overflowMenu = nullptr;
    std::function<QPair<QString, bool>(int)> m_badgeProvider;
};

QtMaterialTabs::QtMaterialTabs(QWidget* parent)
    : QtMaterialTabs(TabsSpec(), parent)
{
}

QtMaterialTabs::QtMaterialTabs(const TabsSpec& spec, QWidget* parent)
    : QTabWidget(parent)
 , d_ptr(std::make_unique<QtMaterialTabsPrivate>())
{
 d_ptr->authoredSpec = spec;
 d_ptr->resolvedSpec = spec;
    setDocumentMode(true);
    setTabBar(new QtMaterialTabsBar(this));
    setMovable(false);
    setTabsClosable(false);

    auto* bar = materialTabBar();
    if (bar) {
        bar->setBadgeProvider([this](int index) {
            const TabDescriptor* d = descriptor(index);
            if (!d) {
                return qMakePair(QString(), false);
            }
            return qMakePair(d->badgeText, d->badgeVisible);
        });
    }

    connect(this, &QTabWidget::currentChanged, this, &QtMaterialTabs::onCurrentTabChanged);
    refreshThemeContextConnection();
    resolveSpecFromTheme();
    applyResolvedSpec();
}

QtMaterialTabs::~QtMaterialTabs() = default;

TabsSpec QtMaterialTabs::spec() const
{
    return authoredSpec();
}

TabsSpec QtMaterialTabs::authoredSpec() const
{
    return d_ptr->authoredSpec;
}

TabsSpec QtMaterialTabs::resolvedSpec() const
{
    return d_ptr->resolvedSpec;
}

void QtMaterialTabs::setSpec(const TabsSpec& spec)
{
    d_ptr->authoredSpec = spec;
    resolveSpecFromTheme();
    applyResolvedSpec();
    emit specChanged();
}

TabsVariant QtMaterialTabs::variant() const { return d_ptr->authoredSpec.variant; }
void QtMaterialTabs::setVariant(TabsVariant value)
{
    if (d_ptr->authoredSpec.variant == value) {
        return;
    }
    d_ptr->authoredSpec.variant = value;
    resolveSpecFromTheme();
    applyResolvedSpec();
    emit variantChanged(value);
}

TabsDensity QtMaterialTabs::density() const { return d_ptr->authoredSpec.density; }
void QtMaterialTabs::setDensity(TabsDensity value)
{
    if (d_ptr->authoredSpec.density == value) {
        return;
    }
    d_ptr->authoredSpec.density = value;
    resolveSpecFromTheme();
    applyResolvedSpec();
    emit densityChanged(value);
}

TabsAlignment QtMaterialTabs::alignment() const { return d_ptr->authoredSpec.alignment; }
void QtMaterialTabs::setAlignment(TabsAlignment value)
{
    if (d_ptr->authoredSpec.alignment == value) {
        return;
    }
    d_ptr->authoredSpec.alignment = value;
    resolveSpecFromTheme();
    applyResolvedSpec();
    emit alignmentChanged(value);
}

bool QtMaterialTabs::isScrollable() const { return d_ptr->authoredSpec.scrollable; }
void QtMaterialTabs::setScrollable(bool value)
{
    if (d_ptr->authoredSpec.scrollable == value) {
        return;
    }
    d_ptr->authoredSpec.scrollable = value;
    resolveSpecFromTheme();
    applyResolvedSpec();
    emit scrollableChanged(value);
}

int QtMaterialTabs::indicatorHeight() const { return d_ptr->authoredSpec.indicatorHeight; }
void QtMaterialTabs::setIndicatorHeight(int value)
{
    value = qMax(1, value);
    if (d_ptr->authoredSpec.indicatorHeight == value) {
        return;
    }
    d_ptr->authoredSpec.indicatorHeight = value;
    resolveSpecFromTheme();
    applyResolvedSpec();
    emit indicatorHeightChanged(value);
}

TabsOverflowMode QtMaterialTabs::overflowMode() const { return d_ptr->authoredSpec.overflowMode; }
void QtMaterialTabs::setOverflowMode(TabsOverflowMode value)
{
    if (d_ptr->authoredSpec.overflowMode == value) {
        return;
    }
    d_ptr->authoredSpec.overflowMode = value;
    resolveSpecFromTheme();
    applyResolvedSpec();
    emit overflowModeChanged(value);
}

bool QtMaterialTabs::usesGlobalTheme() const { return d_ptr->authoredSpec.useGlobalTheme; }
void QtMaterialTabs::setUseGlobalTheme(bool value)
{
    if (d_ptr->authoredSpec.useGlobalTheme == value) {
        return;
    }
    d_ptr->authoredSpec.useGlobalTheme = value;
    resolveSpecFromTheme();
    applyResolvedSpec();
    emit useGlobalThemeChanged(value);
}

void QtMaterialTabs::refreshTheme()
{
    resolveSpecFromTheme();
    applyResolvedSpec();
}

bool QtMaterialTabs::wrapNavigation() const { return d_ptr->authoredSpec.wrapNavigation; }
void QtMaterialTabs::setWrapNavigation(bool value)
{
    if (d_ptr->authoredSpec.wrapNavigation == value) {
        return;
    }
    d_ptr->authoredSpec.wrapNavigation = value;
    resolveSpecFromTheme();
    applyResolvedSpec();
    emit wrapNavigationChanged(value);
}

void QtMaterialTabs::setBadge(int index, const QString& text)
{
    if (TabDescriptor* d = descriptor(index)) {
        d->badgeText = text;
        d->badgeVisible = true;
        if (auto* bar = materialTabBar()) {
            bar->update();
        }
        emit badgeChanged(index);
    }
}

QString QtMaterialTabs::badge(int index) const
{
    const TabDescriptor* d = descriptor(index);
    return d ? d->badgeText : QString();
}

void QtMaterialTabs::setBadgeVisible(int index, bool visible)
{
    if (TabDescriptor* d = descriptor(index)) {
        if (d->badgeVisible == visible) {
            return;
        }
        d->badgeVisible = visible;
        if (auto* bar = materialTabBar()) {
            bar->update();
        }
        emit badgeChanged(index);
    }
}

bool QtMaterialTabs::isBadgeVisible(int index) const
{
    const TabDescriptor* d = descriptor(index);
    return d ? d->badgeVisible : false;
}

void QtMaterialTabs::clearBadge(int index)
{
    if (TabDescriptor* d = descriptor(index)) {
        d->badgeText.clear();
        d->badgeVisible = false;
        if (auto* bar = materialTabBar()) {
            bar->update();
        }
        emit badgeChanged(index);
    }
}

void QtMaterialTabs::setTabId(int index, const QString& id)
{
    if (TabDescriptor* d = descriptor(index)) {
        d->id = id;
        updateAutomationMetadata(index);
        emit tabIdChanged(index, id);
    }
}

QString QtMaterialTabs::tabId(int index) const
{
    const TabDescriptor* d = descriptor(index);
    return d ? d->id : QString();
}

int QtMaterialTabs::indexOfTabId(const QString& id) const
{
    for (int i = 0; i < d_ptr->descriptors.size(); ++i) {
        if (d_ptr->descriptors.at(i).id == id) {
            return i;
        }
    }
    return -1;
}

void QtMaterialTabs::setTabTestId(int index, const QString& testId)
{
    if (TabDescriptor* d = descriptor(index)) {
        d->testId = testId;
        updateAutomationMetadata(index);
        emit tabTestIdChanged(index, testId);
    }
}

QString QtMaterialTabs::tabTestId(int index) const
{
    const TabDescriptor* d = descriptor(index);
    return d ? d->testId : QString();
}

int QtMaterialTabs::indexOfTabTestId(const QString& testId) const
{
    for (int i = 0; i < d_ptr->descriptors.size(); ++i) {
        if (d_ptr->descriptors.at(i).testId == testId) {
            return i;
        }
    }
    return -1;
}

void QtMaterialTabs::setRoute(
    int index,
    const QtMaterialRoute& value)
{
    TabDescriptor* tab = descriptor(index);
    if (!tab) {
        return;
    }

    const QtMaterialRoute normalized(
        value.path());

    if (tab->route == normalized) {
        return;
    }

    tab->route = normalized;
    updateAutomationMetadata(index);

    emit routeChanged(
        index,
        tab->route);

    if (index == currentIndex()) {
        emitCurrentRouteIfChanged();
    }

    syncNavigationModelFromTabs();
}

void QtMaterialTabs::setRoute(int index, const QString& routePath)
{
    setRoute(index, QtMaterialRoute(routePath));
}

QtMaterialRoute QtMaterialTabs::route(int index) const
{
    const TabDescriptor* d = descriptor(index);
    return d ? d->route : QtMaterialRoute();
}

int QtMaterialTabs::indexOfRoute(const QtMaterialRoute& value) const
{
    return indexOfRoute(value.path());
}

int QtMaterialTabs::indexOfRoute(
    const QString& routePath) const
{
    const QString normalized =
        normalizeRoutePath(routePath);

    if (normalized.isEmpty()) {
        return -1;
    }

    for (
        int index = 0;
        index < d_ptr->descriptors.size();
        ++index) {
        if (
            d_ptr->descriptors.at(index)
                .route.path()
            == normalized) {
            return index;
        }
    }

    return -1;
}

bool QtMaterialTabs::navigateTo(const QtMaterialRoute& value)
{
    return navigateTo(value.path());
}

bool QtMaterialTabs::navigateTo(
    const QString& routePath)
{
    const QString normalized =
        normalizeRoutePath(routePath);

    if (normalized.isEmpty()) {
        return false;
    }

    const int index =
        indexOfRoute(normalized);

    if (index < 0) {
        return false;
    }

    ensureTabLoaded(index);

    if (currentIndex() != index) {
        setCurrentIndex(index);
    } else {
        emitCurrentRouteIfChanged();
    }

    return true;
}

bool QtMaterialTabs::navigateToUrl(const QUrl& url)
{
    const QString routePath = routePathFromUrl(url);
    if (routePath.isEmpty()) {
        return false;
    }
    return navigateTo(routePath);
}

bool QtMaterialTabs::lazyLoading() const
{
    return d_ptr->lazyLoading;
}

void QtMaterialTabs::setLazyLoading(bool enabled)
{
    if (d_ptr->lazyLoading == enabled) {
        return;
    }
    d_ptr->lazyLoading = enabled;
    if (enabled && currentIndex() >= 0) {
        ensureTabLoaded(currentIndex());
    }
    emit lazyLoadingChanged(enabled);
}

void QtMaterialTabs::setTabFactory(int index, std::function<QWidget*()> factory)
{
    if (TabDescriptor* d = descriptor(index)) {
        d->factory = std::move(factory);
        d->loaded = false;
        if (!d_ptr->lazyLoading || index == currentIndex()) {
            ensureTabLoaded(index);
        }
    }
}

void QtMaterialTabs::setTabFactory(const QtMaterialRoute& value, std::function<QWidget*()> factory)
{
    const int index = indexOfRoute(value);
    if (index >= 0) {
        setTabFactory(index, std::move(factory));
    }
}

bool QtMaterialTabs::isTabLoaded(int index) const
{
    const TabDescriptor* d = descriptor(index);
    return d ? d->loaded : false;
}

QWidget* QtMaterialTabs::ensureTabLoaded(int index)
{
    TabDescriptor* d = descriptor(index);
    QWidget* page = widget(index);
    if (!d || !page) {
        return page;
    }
    if (d->loaded || !d->factory) {
        return page;
    }

    QWidget* content = d->factory();
    if (!content) {
        return page;
    }

    QLayout* existing = page->layout();
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(existing);
    if (!layout) {
        delete existing;
        layout = new QVBoxLayout(page);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
    } else {
        while (QLayoutItem* item = layout->takeAt(0)) {
            if (QWidget* child = item->widget()) {
                child->deleteLater();
            }
            delete item;
        }
    }

    content->setParent(page);
    layout->addWidget(content);
    d->loaded = true;
    emit tabLoaded(index, content);
    return page;
}

void QtMaterialTabs::bindTo(QStackedWidget* stack)
{
    unbindStackedWidget();
    if (!stack) {
        emit stackBindingChanged(nullptr);
        return;
    }

    d_ptr->boundStack = stack;
    auto* controller = new QtMaterialStackedWidgetController(stack, this);
    bindToController(controller);
    emit stackBindingChanged(stack);
}

void QtMaterialTabs::bindToController(QtMaterialNavigationController* controller)
{
    if (!controller) {
        return;
    }
    for (const auto& existing : d_ptr->boundControllers) {
        if (existing == controller) {
            return;
        }
    }

    d_ptr->boundControllers.push_back(controller);
    connect(controller, &QtMaterialNavigationController::currentIndexChanged,
            this, &QtMaterialTabs::syncCurrentIndexFromController);
    syncCurrentIndexFromController(controller->currentIndex());
    emit controllerBindingChanged(controller, true);
}

void QtMaterialTabs::unbindController(QtMaterialNavigationController* controller)
{
    if (!controller) {
        return;
    }
    disconnect(controller, nullptr, this, nullptr);
    for (int i = 0; i < d_ptr->boundControllers.size(); ++i) {
        if (d_ptr->boundControllers.at(i) == controller) {
            d_ptr->boundControllers.removeAt(i);
            emit controllerBindingChanged(controller, false);
            break;
        }
    }
}

void QtMaterialTabs::unbindAll()
{
    const auto controllers = boundControllers();
    for (auto* controller : controllers) {
        unbindController(controller);
    }
    d_ptr->boundStack = nullptr;
    emit stackBindingChanged(nullptr);
}

void QtMaterialTabs::unbindStackedWidget()
{
    if (!d_ptr->boundStack) {
        return;
    }
    const auto controllers = boundControllers();
    for (auto* controller : controllers) {
        if (auto* stacked = qobject_cast<QtMaterialStackedWidgetController*>(controller)) {
            if (stacked->stackedWidget() == d_ptr->boundStack) {
                unbindController(stacked);
                stacked->deleteLater();
                break;
            }
        }
    }
    d_ptr->boundStack = nullptr;
    emit stackBindingChanged(nullptr);
}

QStackedWidget* QtMaterialTabs::boundStackedWidget() const
{
    return d_ptr->boundStack.data();
}

QVector<QtMaterialNavigationController*> QtMaterialTabs::boundControllers() const
{
    QVector<QtMaterialNavigationController*> out;
    out.reserve(d_ptr->boundControllers.size());
    for (const auto& controller : d_ptr->boundControllers) {
        if (controller) {
            out.push_back(controller);
        }
    }
    return out;
}

void QtMaterialTabs::changeEvent(QEvent* event)
{
    QTabWidget::changeEvent(event);
    if (event->type() == QEvent::PaletteChange) {
        refreshTheme();
    }
}

void QtMaterialTabs::tabInserted(int index)
{
    ensureDescriptorCount(count());
    if (index >= 0 && index < d_ptr->descriptors.size()) {
        d_ptr->descriptors.insert(index, TabDescriptor{});
        while (d_ptr->descriptors.size() > count()) {
            d_ptr->descriptors.removeLast();
        }
    }
    updateAllAutomationMetadata();

    emitCurrentRouteIfChanged();
}

void QtMaterialTabs::tabRemoved(int index)
{
    if (index >= 0 && index < d_ptr->descriptors.size()) {
        d_ptr->descriptors.removeAt(index);
    }
    while (d_ptr->descriptors.size() > count()) {
        d_ptr->descriptors.removeLast();
    }
    updateAllAutomationMetadata();

    emitCurrentRouteIfChanged();
}

void QtMaterialTabs::emitCurrentRouteIfChanged()
{
    const QtMaterialRoute current =
        route(currentIndex());
    const QString path =
        current.path();

    if (
        d_ptr->lastEmittedCurrentRoutePath
        == path) {
        return;
    }

    d_ptr->lastEmittedCurrentRoutePath =
        path;
    emit currentRouteChanged(current);
}

void QtMaterialTabs::onCurrentTabChanged(
    int index)
{
    if (index >= 0) {
        ensureTabLoaded(index);
    }

    syncControllersFromCurrentIndex(index);
    syncNavigationModelSelectionFromCurrentTab();
    syncAccessibilityState();
    emitCurrentRouteIfChanged();
}

QtMaterialTabs::TabDescriptor* QtMaterialTabs::descriptor(int index)
{
    if (index < 0 || index >= d_ptr->descriptors.size()) {
        return nullptr;
    }
    return &d_ptr->descriptors[index];
}

const QtMaterialTabs::TabDescriptor* QtMaterialTabs::descriptor(int index) const
{
    if (index < 0 || index >= d_ptr->descriptors.size()) {
        return nullptr;
    }
    return &d_ptr->descriptors[index];
}


void QtMaterialTabs::setThemeContext(ThemeContext* context)
{
    if (d_ptr->themeContext.data() == context) {
        return;
    }

    d_ptr->themeContext = context;
    const bool effectiveChanged =
        refreshThemeContextConnection();

    emit themeContextChanged(context);
    if (effectiveChanged) {
        emit effectiveThemeContextChanged(
            effectiveThemeContext());
    }

    if (d_ptr->authoredSpec.useGlobalTheme) {
        refreshTheme();
    }
}

ThemeContext* QtMaterialTabs::themeContext() const noexcept
{
    return d_ptr->themeContext.data();
}

ThemeContext* QtMaterialTabs::effectiveThemeContext() const noexcept
{
    if (d_ptr->themeContext) {
        return d_ptr->themeContext.data();
    }

    QWidget* ancestor = parentWidget();
    while (ancestor) {
        if (auto* tabsParent =
                qobject_cast<QtMaterialTabs*>(ancestor)) {
            return tabsParent->effectiveThemeContext();
        }
        if (auto* materialParent =
                qobject_cast<QtMaterialWidget*>(ancestor)) {
            return materialParent->effectiveThemeContext();
        }
        ancestor = ancestor->parentWidget();
    }

    return ThemeManager::instance().defaultContext();
}

bool QtMaterialTabs::event(QEvent* event)
{
    const bool handled = QTabWidget::event(event);

    if (event
        && event->type() == QEvent::ParentChange
        && refreshThemeContextConnection()) {
        emit effectiveThemeContextChanged(
            effectiveThemeContext());

        if (d_ptr->authoredSpec.useGlobalTheme) {
            refreshTheme();
        }
    }

    return handled;
}

bool QtMaterialTabs::refreshThemeContextConnection()
{
    ThemeContext* nextContext = effectiveThemeContext();
    const bool effectiveChanged =
        d_ptr->effectiveThemeContext.data() != nextContext;

    QObject::disconnect(d_ptr->themeChangedConnection);
    QObject::disconnect(d_ptr->themeDestroyedConnection);
    QObject::disconnect(d_ptr->ancestorContextConnection);

    d_ptr->effectiveThemeContext = nextContext;

    if (!d_ptr->themeContext) {
        QWidget* ancestor = parentWidget();
        while (ancestor) {
            if (auto* tabsParent =
                    qobject_cast<QtMaterialTabs*>(ancestor)) {
                d_ptr->ancestorContextConnection =
                    QObject::connect(
                        tabsParent,
                        &QtMaterialTabs::
                            effectiveThemeContextChanged,
                        this,
                        &QtMaterialTabs::
                            handleInheritedThemeContextChanged);
                break;
            }

            if (auto* materialParent =
                    qobject_cast<QtMaterialWidget*>(ancestor)) {
                d_ptr->ancestorContextConnection =
                    QObject::connect(
                        materialParent,
                        &QtMaterialWidget::
                            effectiveThemeContextChanged,
                        this,
                        &QtMaterialTabs::
                            handleInheritedThemeContextChanged);
                break;
            }

            ancestor = ancestor->parentWidget();
        }
    }

    if (nextContext) {
        d_ptr->themeChangedConnection = QObject::connect(
            nextContext,
            &ThemeContext::themeChanged,
            this,
            &QtMaterialTabs::handleThemeChanged);

        const bool explicitContext =
            nextContext == d_ptr->themeContext.data();
        d_ptr->themeDestroyedConnection = QObject::connect(
            nextContext,
            &QObject::destroyed,
            this,
            [this, explicitContext]() {
                handleThemeContextDestroyed(explicitContext);
            });
    }

    return effectiveChanged;
}

void QtMaterialTabs::handleThemeChanged(const Theme&)
{
    if (d_ptr->authoredSpec.useGlobalTheme) {
        refreshTheme();
    }
}

void QtMaterialTabs::handleInheritedThemeContextChanged(
    ThemeContext*)
{
    if (refreshThemeContextConnection()) {
        emit effectiveThemeContextChanged(
            effectiveThemeContext());
    }

    if (d_ptr->authoredSpec.useGlobalTheme) {
        refreshTheme();
    }
}

void QtMaterialTabs::handleThemeContextDestroyed(
    bool explicitContext)
{
    if (explicitContext) {
        d_ptr->themeContext.clear();
        emit themeContextChanged(nullptr);
    }

    d_ptr->effectiveThemeContext.clear();
    refreshThemeContextConnection();

    emit effectiveThemeContextChanged(
        effectiveThemeContext());

    if (d_ptr->authoredSpec.useGlobalTheme) {
        refreshTheme();
    }
}

void QtMaterialTabs::resolveSpecFromTheme()
{
    if (!d_ptr->authoredSpec.useGlobalTheme) {
        d_ptr->resolvedSpec = d_ptr->authoredSpec;
        return;
    }

    ThemeContext* context = effectiveThemeContext();
    Q_ASSERT(context);

    const TabsSpecResolver resolver;
    d_ptr->resolvedSpec =
        resolver.resolve(
            context->theme(),
            d_ptr->authoredSpec);
}

void QtMaterialTabs::applyResolvedSpec()
{
    const QString alignmentRule = d_ptr->resolvedSpec.alignment == TabsAlignment::Center
        ? QStringLiteral("center")
        : QStringLiteral("left");

    setStyleSheet(QStringLiteral(R"(
        QTabWidget::pane {
            border: 0;
            background: %1;
        }
        QTabWidget::tab-bar {
            alignment: %2;
        }
    )")
        .arg(d_ptr->resolvedSpec.containerColor.name(QColor::HexArgb))
        .arg(alignmentRule));

    if (auto* bar = materialTabBar()) {
        bar->setResolvedSpec(d_ptr->resolvedSpec);
        bar->setWrapNavigation(d_ptr->resolvedSpec.wrapNavigation);
    }
}

void QtMaterialTabs::ensureDescriptorCount(int desiredCount)
{
    while (d_ptr->descriptors.size() < desiredCount) {
        d_ptr->descriptors.push_back(TabDescriptor{});
    }
    while (d_ptr->descriptors.size() > desiredCount) {
        d_ptr->descriptors.removeLast();
    }
}

void QtMaterialTabs::updateAutomationMetadata(int index)
{
    if (index < 0 || index >= count()) {
        return;
    }

    if (QWidget* page = widget(index)) {
        const TabDescriptor* d = descriptor(index);
        if (!d) {
            return;
        }
        page->setProperty("materialTabIndex", index);
        page->setProperty("materialTabId", d->id);
        page->setProperty("materialTabTestId", d->testId);
        page->setProperty("materialTabRoute", d->route.path());
        page->setAccessibleName(tabText(index));
    }

    if (QTabBar* bar = tabBar()) {
        bar->setObjectName(QStringLiteral("qtmaterial_tabs_bar"));
        bar->setProperty("materialCurrentTabIndex", currentIndex());
    }
}

void QtMaterialTabs::updateAllAutomationMetadata()
{
    for (int i = 0; i < count(); ++i) {
        updateAutomationMetadata(i);
    }
}

void QtMaterialTabs::syncControllersFromCurrentIndex(int index)
{
    if (d_ptr->syncingExternal) {
        return;
    }
    d_ptr->syncingExternal = true;
    for (const auto& controller : d_ptr->boundControllers) {
        if (controller && controller->currentIndex() != index) {
            controller->setCurrentIndex(index);
        }
    }
    d_ptr->syncingExternal = false;
}

void QtMaterialTabs::syncCurrentIndexFromController(int index)
{
    if (d_ptr->syncingExternal) {
        return;
    }
    if (index < 0 || index >= count()) {
        return;
    }
    d_ptr->syncingExternal = true;
    ensureTabLoaded(index);
    setCurrentIndex(index);
    for (auto& controller : d_ptr->boundControllers) {
        if (controller && controller->currentIndex() != index) {
            controller->setCurrentIndex(index);
        }
    }

    d_ptr->syncingExternal = false;
}

QString QtMaterialTabs::normalizeRoutePath(
    const QString& routePath)
{
    return QtMaterialRoute::normalizedPath(
        routePath);
}

QString QtMaterialTabs::routePathFromUrl(
    const QUrl& url)
{
    if (!url.isValid()) {
        return QString();
    }

    QString routePath;

    if (!url.host().isEmpty()) {
        routePath +=
            QLatin1Char('/')
            + url.host(QUrl::FullyDecoded);
    }

    routePath +=
        url.path(QUrl::FullyDecoded);

    if (routePath.isEmpty()) {
        routePath =
            url.toString(
                QUrl::RemoveQuery
                | QUrl::RemoveFragment);
    }

    return normalizeRoutePath(routePath);
}

QtMaterialTabsBar* QtMaterialTabs::materialTabBar() const
{
    return qobject_cast<QtMaterialTabsBar*>(tabBar());
}


QtMaterialNavigationModel* QtMaterialTabs::navigationModel() const {
    return d_ptr->navigationModel;
}

void QtMaterialTabs::setNavigationModel(QtMaterialNavigationModel* model) {
    if (d_ptr->navigationModel == model) {
        return;
    }

    if (d_ptr->navigationModel) {
        disconnect(d_ptr->navigationModel, nullptr, this, nullptr);
    }

    d_ptr->navigationModel = model;

    if (d_ptr->navigationModel) {
        connect(d_ptr->navigationModel, &QtMaterialNavigationModel::selectedRouteChanged, this, [this](const QString& routePath) {
            if (d_ptr->syncingNavigationModel || routePath.isEmpty()) {
                return;
            }
            d_ptr->syncingNavigationModel = true;
            navigateTo(routePath);
            d_ptr->syncingNavigationModel = false;
        });

        connect(d_ptr->navigationModel, &QtMaterialNavigationModel::selectedIdChanged, this, [this](const QString& id) {
            if (d_ptr->syncingNavigationModel || id.isEmpty()) {
                return;
            }
            const int row = indexOfTabId(id);
            if (row >= 0) {
                d_ptr->syncingNavigationModel = true;
                setCurrentIndex(row);
                d_ptr->syncingNavigationModel = false;
            }
        });
        syncNavigationModelFromTabs();
    }

    emit navigationModelChanged(d_ptr->navigationModel);
}

void QtMaterialTabs::syncNavigationModelFromTabs() {
    if (!d_ptr->navigationModel || d_ptr->syncingNavigationModel) {
        return;
    }

    QVector<QtMaterialNavigationItem> items;
    items.reserve(count());
    for (int i = 0; i < count(); ++i) {
        const TabDescriptor* tab = descriptor(i);

        QtMaterialNavigationItem item;
        item.id = tab && !tab->id.isEmpty()
                      ? tab->id
                      : QString::number(i);

        item.route = tab && tab->route.isValid()
                         ? tab->route.path()
                         : normalizeRoutePath(item.id);


        item.label = tabText(i);
        item.icon = tabIcon(i);
        item.testId = tabTestId(i);
        item.enabled = isTabEnabled(i);
        item.selected = (i == currentIndex());
        items.push_back(item);
    }

    d_ptr->syncingNavigationModel = true;
    d_ptr->navigationModel->setItems(items);
    syncNavigationModelSelectionFromCurrentTab();
    d_ptr->syncingNavigationModel = false;
}

void QtMaterialTabs::syncNavigationModelSelectionFromCurrentTab()
{
    if (!d_ptr->navigationModel || d_ptr->syncingNavigationModel) {
        return;
    }

    const TabDescriptor* d = descriptor(currentIndex());
    if (!d) {
        return;
    }

    d_ptr->syncingNavigationModel = true;

    bool updated = false;

    if (!d->route.path().isEmpty()) {
        updated = d_ptr->navigationModel->setSelectedRoute(d->route.path());
    }

    if (!updated && !d->id.isEmpty()) {
        updated = d_ptr->navigationModel->setSelectedId(d->id);
    }

    d_ptr->syncingNavigationModel = false;
}

void QtMaterial::QtMaterialTabs::syncAccessibilityState()
{
    const QString summary = accessibilitySummary();

    if (accessibleName().trimmed().isEmpty()) {
        setAccessibleName(tr("Tabs"));
    }
    setAccessibleDescription(summary);

    if (auto* bar = tabBar()) {
        if (bar->accessibleName().trimmed().isEmpty()) {
            bar->setAccessibleName(tr("Tabs"));
        }
        bar->setAccessibleDescription(summary);
    }

    if (d_ptr->lastAccessibilitySummary == summary) {
        return;
    }

    d_ptr->lastAccessibilitySummary = summary;
    emit accessibilitySummaryChanged(summary);
}

QString QtMaterial::QtMaterialTabs::accessibilitySummary() const
{
    if (count() <= 0) {
        return tr("No tabs");
    }

    const int index = currentIndex();
    if (index < 0 || index >= count()) {
        return tr("%1 tabs").arg(count());
    }

    return tr("%1 of %2: %3").arg(index + 1).arg(count()).arg(currentTabAccessibleText());
}

QString QtMaterial::QtMaterialTabs::currentTabAccessibleText() const
{
    const int index = currentIndex();
    if (index < 0 || index >= count()) {
        return QString();
    }

    QString label = tabText(index).trimmed();
    if (label.isEmpty()) {
        if (const TabDescriptor* d = descriptor(index)) {
            if (!d->id.trimmed().isEmpty()) {
                label = d->id.trimmed();
            } else if (d->route.isValid()) {
                label = d->route.toString();
            }
        }
    }

    if (label.isEmpty()) {
        label = tr("Tab %1").arg(index + 1);
    }

    if (isBadgeVisible(index)) {
        const QString badgeText = badge(index).trimmed();
        label += badgeText.isEmpty() ? tr(", has updates") : tr(", badge %1").arg(badgeText);
    }

    return label;
}

} // namespace QtMaterial

#include "qtmaterialtabs.moc"
#include "qtmaterial/core/qtmaterialwidget.h"
#include "qtmaterial/specs/qtmaterialtabsspecresolver.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"
#include <QMetaObject>
