#include "qtmaterial/widgets/layouts/qtmaterialadaptiveshell.h"

#include <QEvent>
#include <QPointer>
#include <QResizeEvent>

#include "qtmaterial/core/qtmaterialabstractbutton.h"
#include "qtmaterial/core/qtmaterialcontrol.h"
#include "qtmaterial/widgets/navigation/qtmaterialnavigationsuite.h"

namespace QtMaterial {

struct QtMaterialAdaptiveShellPrivate
{
    QtMaterialNavigationSuite* navigation = nullptr;
    QPointer<QWidget> content;
    QPointer<QWidget> supporting;
    WindowSizeClass sizeClass;
    Density density = Density::Default;
    bool automaticDensity = true;
    int supportingPaneWidth = 360;
};

namespace {

Density densityForWidthClass(WindowWidthSizeClass sizeClass) noexcept
{
    switch (sizeClass) {
    case WindowWidthSizeClass::Compact:
        return Density::Default;
    case WindowWidthSizeClass::Medium:
        return Density::Comfortable;
    case WindowWidthSizeClass::Expanded:
    case WindowWidthSizeClass::Large:
    case WindowWidthSizeClass::ExtraLarge:
        return Density::Compact;
    }
    return Density::Default;
}

bool supportsSecondaryPane(WindowWidthSizeClass sizeClass) noexcept
{
    return sizeClass == WindowWidthSizeClass::Expanded
        || sizeClass == WindowWidthSizeClass::Large
        || sizeClass == WindowWidthSizeClass::ExtraLarge;
}

} // namespace

QtMaterialAdaptiveShell::QtMaterialAdaptiveShell(QWidget* parent)
    : QtMaterialWidget(parent)
    , d_ptr(std::make_unique<QtMaterialAdaptiveShellPrivate>())
{
    d_ptr->navigation = new QtMaterialNavigationSuite(this);
    d_ptr->navigation->show();

    setMaterialComponent(QStringLiteral("adaptive-shell"));
    setMaterialVariant(QStringLiteral("responsive"));
    setMaterialRole(QStringLiteral("layout"));

    applyAdaptiveState();
}

QtMaterialAdaptiveShell::~QtMaterialAdaptiveShell() = default;

QtMaterialNavigationSuite* QtMaterialAdaptiveShell::navigationSuite() const noexcept
{
    return d_ptr->navigation;
}

QWidget* QtMaterialAdaptiveShell::contentWidget() const noexcept
{
    return d_ptr->content.data();
}

void QtMaterialAdaptiveShell::setContentWidget(QWidget* widget)
{
    if (d_ptr->content == widget) {
        return;
    }

    if (d_ptr->content) {
        d_ptr->content->hide();
        d_ptr->content->setParent(nullptr);
    }

    d_ptr->content = widget;
    if (d_ptr->content) {
        d_ptr->content->setParent(this);
        d_ptr->content->show();
    }

    if (d_ptr->automaticDensity) {
        applyDensityToChildren();
    }
    applyAdaptiveGeometry();
}

QWidget* QtMaterialAdaptiveShell::supportingWidget() const noexcept
{
    return d_ptr->supporting.data();
}

void QtMaterialAdaptiveShell::setSupportingWidget(QWidget* widget)
{
    if (d_ptr->supporting == widget) {
        return;
    }

    if (d_ptr->supporting) {
        d_ptr->supporting->hide();
        d_ptr->supporting->setParent(nullptr);
    }

    d_ptr->supporting = widget;
    if (d_ptr->supporting) {
        d_ptr->supporting->setParent(this);
    }

    if (d_ptr->automaticDensity) {
        applyDensityToChildren();
    }
    applyAdaptiveGeometry();
}

WindowSizeClass QtMaterialAdaptiveShell::windowSizeClass() const noexcept
{
    return d_ptr->sizeClass;
}

Density QtMaterialAdaptiveShell::resolvedDensity() const noexcept
{
    return d_ptr->density;
}

bool QtMaterialAdaptiveShell::automaticDensity() const noexcept
{
    return d_ptr->automaticDensity;
}

void QtMaterialAdaptiveShell::setAutomaticDensity(bool enabled)
{
    if (d_ptr->automaticDensity == enabled) {
        return;
    }

    d_ptr->automaticDensity = enabled;
    if (enabled) {
        applyDensityToChildren();
    }
    emit automaticDensityChanged(enabled);
}

int QtMaterialAdaptiveShell::supportingPaneWidth() const noexcept
{
    return d_ptr->supportingPaneWidth;
}

void QtMaterialAdaptiveShell::setSupportingPaneWidth(int width)
{
    const int normalized = qMax(240, width);
    if (d_ptr->supportingPaneWidth == normalized) {
        return;
    }

    d_ptr->supportingPaneWidth = normalized;
    applyAdaptiveGeometry();
}

void QtMaterialAdaptiveShell::resizeEvent(QResizeEvent* event)
{
    QtMaterialWidget::resizeEvent(event);
    applyAdaptiveState();
}

void QtMaterialAdaptiveShell::changeEvent(QEvent* event)
{
    QtMaterialWidget::changeEvent(event);
    if (event && event->type() == QEvent::LayoutDirectionChange) {
        applyAdaptiveGeometry();
    }
}

void QtMaterialAdaptiveShell::applyAdaptiveState()
{
    const WindowSizeClass next =
        WindowSizeClass::fromLogicalSize(width(), height());
    const WindowSizeClass previous = d_ptr->sizeClass;

    d_ptr->sizeClass = next;
    d_ptr->navigation->setWindowWidthSizeClass(next.width);

    if (previous.width != next.width) {
        emit widthSizeClassChanged(next.width);
    }
    if (previous.height != next.height) {
        emit heightSizeClassChanged(next.height);
    }

    const Density nextDensity = densityForWidthClass(next.width);
    if (d_ptr->density != nextDensity) {
        d_ptr->density = nextDensity;
        emit resolvedDensityChanged(nextDensity);
    }

    if (d_ptr->automaticDensity) {
        applyDensityToChildren();
    }

    applyAdaptiveGeometry();
}

void QtMaterialAdaptiveShell::applyDensityToChildren()
{
    const auto controls = findChildren<QtMaterialControl*>();
    for (QtMaterialControl* control : controls) {
        if (control) {
            control->setDensity(d_ptr->density);
        }
    }

    const auto buttons = findChildren<QtMaterialAbstractButton*>();
    for (QtMaterialAbstractButton* button : buttons) {
        if (button) {
            button->setDensity(d_ptr->density);
        }
    }
}

void QtMaterialAdaptiveShell::applyAdaptiveGeometry()
{
    if (!d_ptr->navigation) {
        return;
    }

    const QRect bounds = contentsRect();
    if (bounds.isEmpty()) {
        return;
    }

    const bool bar =
        d_ptr->navigation->navigationType() == NavigationSuiteType::NavigationBar;

    if (bar) {
        const int navigationHeight = qMin(80, bounds.height());
        d_ptr->navigation->setGeometry(
            bounds.left(),
            bounds.bottom() - navigationHeight + 1,
            bounds.width(),
            navigationHeight);

        if (d_ptr->content) {
            d_ptr->content->setGeometry(
                bounds.left(),
                bounds.top(),
                bounds.width(),
                qMax(0, bounds.height() - navigationHeight));
            d_ptr->content->show();
        }

        if (d_ptr->supporting) {
            d_ptr->supporting->hide();
        }
        return;
    }

    const int navigationWidth =
        qMin(d_ptr->navigation->sizeHint().width(), bounds.width());
    const bool rtl = layoutDirection() == Qt::RightToLeft;

    const int navigationX = rtl
        ? bounds.right() - navigationWidth + 1
        : bounds.left();
    d_ptr->navigation->setGeometry(
        navigationX,
        bounds.top(),
        navigationWidth,
        bounds.height());

    QRect body = bounds;
    if (rtl) {
        body.setRight(navigationX - 1);
    } else {
        body.setLeft(navigationX + navigationWidth);
    }

    bool showSupporting =
        d_ptr->supporting
        && supportsSecondaryPane(d_ptr->sizeClass.width);

    constexpr int paneGap = 16;
    constexpr int minimumContentWidth = 320;
    constexpr int minimumSupportingWidth = 240;

    int supportWidth = 0;
    if (showSupporting) {
        const int availableForSupporting =
            body.width() - minimumContentWidth - paneGap;
        supportWidth =
            qMin(d_ptr->supportingPaneWidth, availableForSupporting);
        showSupporting = supportWidth >= minimumSupportingWidth;
    }

    if (showSupporting) {
        QRect supportRect;
        QRect contentRect = body;

        if (rtl) {
            supportRect = QRect(
                body.left(),
                body.top(),
                supportWidth,
                body.height());
            contentRect.setLeft(supportRect.right() + 1 + paneGap);
        } else {
            supportRect = QRect(
                body.right() - supportWidth + 1,
                body.top(),
                supportWidth,
                body.height());
            contentRect.setRight(supportRect.left() - 1 - paneGap);
        }

        d_ptr->supporting->setGeometry(supportRect);
        d_ptr->supporting->show();

        if (d_ptr->content) {
            d_ptr->content->setGeometry(contentRect);
            d_ptr->content->show();
        }
        return;
    }

    if (d_ptr->supporting) {
        d_ptr->supporting->hide();
    }

    if (d_ptr->content) {
        d_ptr->content->setGeometry(body);
        d_ptr->content->show();
    }
}

} // namespace QtMaterial
