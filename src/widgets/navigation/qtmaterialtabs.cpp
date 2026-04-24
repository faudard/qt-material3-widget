#include "qtmaterial/widgets/navigation/qtmaterialtabs.h"

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

#include "qtmaterial/theme/qtmaterialcolorscheme.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include "qtmaterial/theme/qtmaterialstatelayer.h"
#include "qtmaterial/theme/qtmaterialtheme.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/widgets/navigation/qtmaterialnavigationcontroller.h"

namespace QtMaterial {
namespace {

QColor withAlpha(QColor color, qreal opacity)
{
    color.setAlphaF(qBound<qreal>(0.0, opacity, 1.0));
    return color;
}

QColor roleOrFallback(const ColorScheme& scheme, ColorRole role, const QColor& fallback)
{
    return scheme.contains(role) ? scheme.color(role) : fallback;
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
    , m_authoredSpec(spec)
    , m_resolvedSpec(spec)
{
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
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, [this](const Theme&) {
        if (m_authoredSpec.useGlobalTheme) {
            refreshTheme();
        }
    });

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
    return m_authoredSpec;
}

TabsSpec QtMaterialTabs::resolvedSpec() const
{
    return m_resolvedSpec;
}

void QtMaterialTabs::setSpec(const TabsSpec& spec)
{
    m_authoredSpec = spec;
    resolveSpecFromTheme();
    applyResolvedSpec();
    emit specChanged();
}

TabsVariant QtMaterialTabs::variant() const { return m_authoredSpec.variant; }
void QtMaterialTabs::setVariant(TabsVariant value)
{
    if (m_authoredSpec.variant == value) {
        return;
    }
    m_authoredSpec.variant = value;
    resolveSpecFromTheme();
    applyResolvedSpec();
    emit variantChanged(value);
}

TabsDensity QtMaterialTabs::density() const { return m_authoredSpec.density; }
void QtMaterialTabs::setDensity(TabsDensity value)
{
    if (m_authoredSpec.density == value) {
        return;
    }
    m_authoredSpec.density = value;
    resolveSpecFromTheme();
    applyResolvedSpec();
    emit densityChanged(value);
}

TabsAlignment QtMaterialTabs::alignment() const { return m_authoredSpec.alignment; }
void QtMaterialTabs::setAlignment(TabsAlignment value)
{
    if (m_authoredSpec.alignment == value) {
        return;
    }
    m_authoredSpec.alignment = value;
    resolveSpecFromTheme();
    applyResolvedSpec();
    emit alignmentChanged(value);
}

bool QtMaterialTabs::isScrollable() const { return m_authoredSpec.scrollable; }
void QtMaterialTabs::setScrollable(bool value)
{
    if (m_authoredSpec.scrollable == value) {
        return;
    }
    m_authoredSpec.scrollable = value;
    resolveSpecFromTheme();
    applyResolvedSpec();
    emit scrollableChanged(value);
}

int QtMaterialTabs::indicatorHeight() const { return m_authoredSpec.indicatorHeight; }
void QtMaterialTabs::setIndicatorHeight(int value)
{
    value = qMax(1, value);
    if (m_authoredSpec.indicatorHeight == value) {
        return;
    }
    m_authoredSpec.indicatorHeight = value;
    resolveSpecFromTheme();
    applyResolvedSpec();
    emit indicatorHeightChanged(value);
}

TabsOverflowMode QtMaterialTabs::overflowMode() const { return m_authoredSpec.overflowMode; }
void QtMaterialTabs::setOverflowMode(TabsOverflowMode value)
{
    if (m_authoredSpec.overflowMode == value) {
        return;
    }
    m_authoredSpec.overflowMode = value;
    resolveSpecFromTheme();
    applyResolvedSpec();
    emit overflowModeChanged(value);
}

bool QtMaterialTabs::usesGlobalTheme() const { return m_authoredSpec.useGlobalTheme; }
void QtMaterialTabs::setUseGlobalTheme(bool value)
{
    if (m_authoredSpec.useGlobalTheme == value) {
        return;
    }
    m_authoredSpec.useGlobalTheme = value;
    resolveSpecFromTheme();
    applyResolvedSpec();
    emit useGlobalThemeChanged(value);
}

void QtMaterialTabs::refreshTheme()
{
    resolveSpecFromTheme();
    applyResolvedSpec();
}

bool QtMaterialTabs::wrapNavigation() const { return m_authoredSpec.wrapNavigation; }
void QtMaterialTabs::setWrapNavigation(bool value)
{
    if (m_authoredSpec.wrapNavigation == value) {
        return;
    }
    m_authoredSpec.wrapNavigation = value;
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
    for (int i = 0; i < m_descriptors.size(); ++i) {
        if (m_descriptors.at(i).id == id) {
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
    for (int i = 0; i < m_descriptors.size(); ++i) {
        if (m_descriptors.at(i).testId == testId) {
            return i;
        }
    }
    return -1;
}

void QtMaterialTabs::setRoute(int index, const QtMaterialRoute& value)
{
    if (TabDescriptor* d = descriptor(index)) {
        d->route = QtMaterialRoute(normalizeRoutePath(value.path()));
        updateAutomationMetadata(index);
        emit routeChanged(index, d->route);
        if (index == currentIndex()) {
            emit currentRouteChanged(d->route);
        }
    }
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

int QtMaterialTabs::indexOfRoute(const QString& routePath) const
{
    const QString normalized = normalizeRoutePath(routePath);
    for (int i = 0; i < m_descriptors.size(); ++i) {
        if (m_descriptors.at(i).route.path() == normalized) {
            return i;
        }
    }
    return -1;
}

bool QtMaterialTabs::navigateTo(const QtMaterialRoute& value)
{
    return navigateTo(value.path());
}

bool QtMaterialTabs::navigateTo(const QString& routePath)
{
    const int index = indexOfRoute(routePath);
    if (index < 0) {
        return false;
    }
    ensureTabLoaded(index);
    setCurrentIndex(index);
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
    return m_lazyLoading;
}

void QtMaterialTabs::setLazyLoading(bool enabled)
{
    if (m_lazyLoading == enabled) {
        return;
    }
    m_lazyLoading = enabled;
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
        if (!m_lazyLoading || index == currentIndex()) {
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

    m_boundStack = stack;
    auto* controller = new QtMaterialStackedWidgetController(stack, this);
    bindToController(controller);
    emit stackBindingChanged(stack);
}

void QtMaterialTabs::bindToController(QtMaterialNavigationController* controller)
{
    if (!controller) {
        return;
    }
    for (const auto& existing : m_boundControllers) {
        if (existing == controller) {
            return;
        }
    }

    m_boundControllers.push_back(controller);
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
    for (int i = 0; i < m_boundControllers.size(); ++i) {
        if (m_boundControllers.at(i) == controller) {
            m_boundControllers.removeAt(i);
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
    m_boundStack = nullptr;
    emit stackBindingChanged(nullptr);
}

void QtMaterialTabs::unbindStackedWidget()
{
    if (!m_boundStack) {
        return;
    }
    const auto controllers = boundControllers();
    for (auto* controller : controllers) {
        if (auto* stacked = qobject_cast<QtMaterialStackedWidgetController*>(controller)) {
            if (stacked->stackedWidget() == m_boundStack) {
                unbindController(stacked);
                stacked->deleteLater();
                break;
            }
        }
    }
    m_boundStack = nullptr;
    emit stackBindingChanged(nullptr);
}

QStackedWidget* QtMaterialTabs::boundStackedWidget() const
{
    return m_boundStack.data();
}

QVector<QtMaterialNavigationController*> QtMaterialTabs::boundControllers() const
{
    QVector<QtMaterialNavigationController*> out;
    out.reserve(m_boundControllers.size());
    for (const auto& controller : m_boundControllers) {
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
    if (index >= 0 && index < m_descriptors.size()) {
        m_descriptors.insert(index, TabDescriptor{});
        while (m_descriptors.size() > count()) {
            m_descriptors.removeLast();
        }
    }
    updateAllAutomationMetadata();
}

void QtMaterialTabs::tabRemoved(int index)
{
    if (index >= 0 && index < m_descriptors.size()) {
        m_descriptors.removeAt(index);
    }
    while (m_descriptors.size() > count()) {
        m_descriptors.removeLast();
    }
    updateAllAutomationMetadata();
}

void QtMaterialTabs::onCurrentTabChanged(int index)
{
    ensureTabLoaded(index);
    syncControllersFromCurrentIndex(index);
    if (const TabDescriptor* d = descriptor(index)) {
        emit currentRouteChanged(d->route);
    }
}

QtMaterialTabs::TabDescriptor* QtMaterialTabs::descriptor(int index)
{
    if (index < 0 || index >= m_descriptors.size()) {
        return nullptr;
    }
    return &m_descriptors[index];
}

const QtMaterialTabs::TabDescriptor* QtMaterialTabs::descriptor(int index) const
{
    if (index < 0 || index >= m_descriptors.size()) {
        return nullptr;
    }
    return &m_descriptors[index];
}

void QtMaterialTabs::resolveSpecFromTheme()
{
    m_resolvedSpec = m_authoredSpec;

    if (!m_authoredSpec.useGlobalTheme) {
        return;
    }

    const QPalette palette = this->palette();
    const Theme& theme = ThemeManager::instance().theme();
    const ColorScheme& scheme = theme.colorScheme();
    const StateLayer& stateLayer = theme.stateLayer();

    const QColor primary = roleOrFallback(scheme, ColorRole::Primary, palette.color(QPalette::Highlight));
    const QColor onSurface = roleOrFallback(scheme, ColorRole::OnSurface, palette.color(QPalette::WindowText));
    const QColor onSurfaceVariant = roleOrFallback(scheme, ColorRole::OnSurfaceVariant, palette.color(QPalette::WindowText));
    const QColor surface = roleOrFallback(scheme, ColorRole::Surface, Qt::transparent);
    const QColor outline = roleOrFallback(scheme, ColorRole::Outline, palette.color(QPalette::Mid));
    const QColor error = roleOrFallback(scheme, ColorRole::Error, QColor(QStringLiteral("#B3261E")));
    const QColor onError = roleOrFallback(scheme, ColorRole::OnError, Qt::white);

    if (!m_resolvedSpec.containerColor.isValid()) {
        m_resolvedSpec.containerColor = surface;
    }
    if (!m_resolvedSpec.inactiveLabelColor.isValid()) {
        m_resolvedSpec.inactiveLabelColor = onSurfaceVariant;
    }
    if (!m_resolvedSpec.activeLabelColor.isValid()) {
        m_resolvedSpec.activeLabelColor = (m_authoredSpec.variant == TabsVariant::Primary) ? primary : onSurface;
    }
    if (!m_resolvedSpec.activeIndicatorColor.isValid()) {
        m_resolvedSpec.activeIndicatorColor = primary;
    }
    if (!m_resolvedSpec.stateLayerColor.isValid()) {
        m_resolvedSpec.stateLayerColor = m_resolvedSpec.activeLabelColor;
    }
    if (!m_resolvedSpec.hoverStateLayerColor.isValid()) {
        m_resolvedSpec.hoverStateLayerColor = stateLayer.color.isValid() ? stateLayer.color : m_resolvedSpec.stateLayerColor;
    }
    if (!m_resolvedSpec.focusedStateLayerColor.isValid()) {
        m_resolvedSpec.focusedStateLayerColor = stateLayer.color.isValid() ? stateLayer.color : m_resolvedSpec.stateLayerColor;
    }
    if (!m_resolvedSpec.pressedStateLayerColor.isValid()) {
        m_resolvedSpec.pressedStateLayerColor = stateLayer.color.isValid() ? stateLayer.color : m_resolvedSpec.stateLayerColor;
    }
    if (!m_resolvedSpec.focusRingColor.isValid()) {
        m_resolvedSpec.focusRingColor = outline;
    }
    if (!m_resolvedSpec.disabledLabelColor.isValid()) {
        m_resolvedSpec.disabledLabelColor = palette.color(QPalette::Disabled, QPalette::WindowText);
    }
    if (!m_resolvedSpec.badgeColor.isValid()) {
        m_resolvedSpec.badgeColor = error;
    }
    if (!m_resolvedSpec.badgeLabelColor.isValid()) {
        m_resolvedSpec.badgeLabelColor = onError;
    }
    if (!m_resolvedSpec.overflowButtonColor.isValid()) {
        m_resolvedSpec.overflowButtonColor = onSurfaceVariant;
    }

    if (m_resolvedSpec.hoverOpacity < 0.0) {
        m_resolvedSpec.hoverOpacity = stateLayer.hoverOpacity;
    }
    if (m_resolvedSpec.focusOpacity < 0.0) {
        m_resolvedSpec.focusOpacity = stateLayer.focusOpacity;
    }
    if (m_resolvedSpec.pressedOpacity < 0.0) {
        m_resolvedSpec.pressedOpacity = stateLayer.pressOpacity;
    }
}

void QtMaterialTabs::applyResolvedSpec()
{
    const QString alignmentRule = m_resolvedSpec.alignment == TabsAlignment::Center
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
        .arg(m_resolvedSpec.containerColor.name(QColor::HexArgb))
        .arg(alignmentRule));

    if (auto* bar = materialTabBar()) {
        bar->setResolvedSpec(m_resolvedSpec);
        bar->setWrapNavigation(m_resolvedSpec.wrapNavigation);
    }
}

void QtMaterialTabs::ensureDescriptorCount(int desiredCount)
{
    while (m_descriptors.size() < desiredCount) {
        m_descriptors.push_back(TabDescriptor{});
    }
    while (m_descriptors.size() > desiredCount) {
        m_descriptors.removeLast();
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
    if (m_syncingExternal) {
        return;
    }
    m_syncingExternal = true;
    for (const auto& controller : m_boundControllers) {
        if (controller && controller->currentIndex() != index) {
            controller->setCurrentIndex(index);
        }
    }
    m_syncingExternal = false;
}

void QtMaterialTabs::syncCurrentIndexFromController(int index)
{
    if (m_syncingExternal) {
        return;
    }
    if (index < 0 || index >= count()) {
        return;
    }
    m_syncingExternal = true;
    ensureTabLoaded(index);
    setCurrentIndex(index);
    m_syncingExternal = false;
}

QString QtMaterialTabs::normalizeRoutePath(const QString& routePath)
{
    QString value = routePath.trimmed();
    while (value.startsWith(QLatin1Char('/'))) {
        value.remove(0, 1);
    }
    while (value.endsWith(QLatin1Char('/'))) {
        value.chop(1);
    }
    return value;
}

QString QtMaterialTabs::routePathFromUrl(const QUrl& url)
{
    if (!url.isValid()) {
        return QString();
    }
    QString path;
    if (!url.host().isEmpty()) {
        path += url.host();
    }
    if (!url.path().isEmpty()) {
        if (!path.isEmpty() && !url.path().startsWith(QLatin1Char('/'))) {
            path += QLatin1Char('/');
        }
        path += url.path();
    }
    return normalizeRoutePath(path);
}

QtMaterialTabsBar* QtMaterialTabs::materialTabBar() const
{
    return qobject_cast<QtMaterialTabsBar*>(tabBar());
}

} // namespace QtMaterial

#include "qtmaterialtabs.moc"
