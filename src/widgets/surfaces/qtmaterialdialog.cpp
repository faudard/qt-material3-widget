#include "qtmaterial/widgets/surfaces/qtmaterialdialog.h"

#include <QAbstractButton>
#include <QAccessible>
#include <QApplication>
#include <QKeyEvent>
#include <QPainter>
#include <QVBoxLayout>
#include <QWidget>

#include <algorithm>

#include "qtmaterial/effects/qtmaterialscrimwidget.h"
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include <memory>
#include "qtmaterial/specs/qtmaterialdialogspec.h"
#include "qtmaterial/effects/qtmaterialelevationrenderer.h"
#include "qtmaterial/specs/qtmaterialdialogspecresolver.h"

namespace QtMaterial {


class QtMaterialDialogPrivate {
public:
 mutable bool specDirty = true;
 mutable DialogSpec spec;
 QString titleText;
 QString supportingText;
 QPointer<QWidget> bodyWidget;
 QPointer<QWidget> initialFocusWidget;
 QPointer<QWidget> previousFocusWidget;
 QPointer<QAbstractButton> defaultButton;
 QVBoxLayout* layout = nullptr;
 QtMaterialScrimWidget* scrim = nullptr;
 QtMaterialTransitionController* transition = nullptr;
 bool dismissOnEscape = true;
 bool restoreFocusOnClose = true;
};

QtMaterialDialog::QtMaterialDialog(QWidget* parent)
    : QtMaterialOverlaySurface(parent)
 , d_ptr(std::make_unique<QtMaterialDialogPrivate>())
{
 d_ptr->layout = new QVBoxLayout(this);
 d_ptr->scrim = new QtMaterialScrimWidget(parent ? parent : this);
 d_ptr->transition = new QtMaterialTransitionController(this);
    setObjectName(QStringLiteral("qtmaterial_dialog"));
    setFocusPolicy(Qt::StrongFocus);
    setAccessibleName(QStringLiteral("Dialog"));

    d_ptr->layout->setContentsMargins(24, 24, 24, 24);

    hide();
    if (d_ptr->scrim) {
        d_ptr->scrim->hide();
    }

    updateAccessibilityMetadata();
}

QtMaterialDialog::~QtMaterialDialog() = default;

QString QtMaterialDialog::titleText() const
{
    return d_ptr->titleText;
}

void QtMaterialDialog::setTitleText(const QString& title)
{
    if (d_ptr->titleText == title) {
        return;
    }

    d_ptr->titleText = title;
    updateAccessibilityMetadata();
}

QString QtMaterialDialog::supportingText() const
{
    return d_ptr->supportingText;
}

void QtMaterialDialog::setSupportingText(const QString& text)
{
    if (d_ptr->supportingText == text) {
        return;
    }

    d_ptr->supportingText = text;
    updateAccessibilityMetadata();
}

void QtMaterialDialog::setBodyWidget(QWidget* widget)
{
    if (d_ptr->bodyWidget == widget) {
        return;
    }

    if (d_ptr->bodyWidget) {
        d_ptr->layout->removeWidget(d_ptr->bodyWidget);
        d_ptr->bodyWidget->setParent(nullptr);
    }

    d_ptr->bodyWidget = widget;

    if (d_ptr->bodyWidget) {
        d_ptr->bodyWidget->setParent(this);
        d_ptr->layout->addWidget(d_ptr->bodyWidget);

        if (!d_ptr->initialFocusWidget) {
            d_ptr->initialFocusWidget = d_ptr->bodyWidget;
        }
    }

    updateAccessibilityMetadata();
}

QWidget* QtMaterialDialog::bodyWidget() const
{
    return d_ptr->bodyWidget;
}

QWidget* QtMaterialDialog::initialFocusWidget() const
{
    return d_ptr->initialFocusWidget;
}

void QtMaterialDialog::setInitialFocusWidget(QWidget* widget)
{
    if (d_ptr->initialFocusWidget == widget) {
        return;
    }

    d_ptr->initialFocusWidget = widget;
}

QAbstractButton* QtMaterialDialog::defaultButton() const
{
    return d_ptr->defaultButton;
}

void QtMaterialDialog::setDefaultButton(QAbstractButton* button)
{
    if (d_ptr->defaultButton == button) {
        return;
    }

    d_ptr->defaultButton = button;
}

bool QtMaterialDialog::dismissOnEscape() const noexcept
{
    return d_ptr->dismissOnEscape;
}

void QtMaterialDialog::setDismissOnEscape(bool enabled)
{
    d_ptr->dismissOnEscape = enabled;
}

bool QtMaterialDialog::restoreFocusOnClose() const noexcept
{
    return d_ptr->restoreFocusOnClose;
}

void QtMaterialDialog::setRestoreFocusOnClose(bool enabled)
{
    d_ptr->restoreFocusOnClose = enabled;
}

QString QtMaterialDialog::accessibilitySummary() const
{
    if (!d_ptr->titleText.isEmpty() && !d_ptr->supportingText.isEmpty()) {
        return d_ptr->titleText + QStringLiteral(". ") + d_ptr->supportingText;
    }

    if (!d_ptr->titleText.isEmpty()) {
        return d_ptr->titleText;
    }

    if (!d_ptr->supportingText.isEmpty()) {
        return d_ptr->supportingText;
    }

    return QStringLiteral("Dialog");
}

void QtMaterialDialog::open()
{
    d_ptr->previousFocusWidget =
        QApplication::focusWidget();

    resolveSpecIfNeeded();
    syncChildGeometry();

    if (d_ptr->scrim) {
        d_ptr->scrim->setScrimColor(
            d_ptr->spec.scrimColor);
        d_ptr->scrim->setGeometry(
            parentWidget()
            ? parentWidget()->rect()
            : rect());
        d_ptr->scrim->show();
        d_ptr->scrim->raise();
    }

    show();
    raise();
    activateWindow();

    if (d_ptr->transition) {
        if (d_ptr->spec.hasResolvedEnterMotionStyle) {
            d_ptr->transition->applyMotionStyle(
                d_ptr->spec.enterMotionStyle);
        }
        d_ptr->transition->startForward();
    }

    focusInitialWidget();
    Q_EMIT opened();
}

void QtMaterialDialog::close()
{
    resolveSpecIfNeeded();

    if (d_ptr->transition) {
        if (d_ptr->spec.hasResolvedExitMotionStyle) {
            d_ptr->transition->applyMotionStyle(
                d_ptr->spec.exitMotionStyle);
        }
        d_ptr->transition->startBackward();
    }

    hide();

    if (d_ptr->scrim) {
        d_ptr->scrim->hide();
    }

    if (d_ptr->restoreFocusOnClose
        && d_ptr->previousFocusWidget) {
        d_ptr->previousFocusWidget->setFocus(
            Qt::OtherFocusReason);
    }

    Q_EMIT closed();
}

void QtMaterialDialog::reject()
{
    close();
    Q_EMIT rejected();
}

void QtMaterialDialog::themeChangedEvent(
    const Theme& changedTheme)
{
    QtMaterialOverlaySurface::themeChangedEvent(
        changedTheme);
    d_ptr->specDirty = true;
    resolveSpecIfNeeded();
    updateGeometry();
    update();
}

void QtMaterialDialog::invalidateResolvedSpec()
{
    d_ptr->specDirty = true;
}

void QtMaterialDialog::resolveSpecIfNeeded() const
{
    if (!d_ptr->specDirty) {
        return;
    }

    const DialogSpecResolver resolver;
    d_ptr->spec = resolver.dialogSpec(theme());
    d_ptr->specDirty = false;

    if (d_ptr->layout) {
        d_ptr->layout->setContentsMargins(
            d_ptr->spec.padding,
            d_ptr->spec.padding,
            d_ptr->spec.padding,
            d_ptr->spec.padding);
    }

    if (d_ptr->scrim) {
        d_ptr->scrim->setScrimColor(
            d_ptr->spec.scrimColor);
    }
}

void QtMaterialDialog::syncChildGeometry()
{
    resolveSpecIfNeeded();

    const int maximumWidth = qMax(1, d_ptr->spec.maxWidth);
    const int dialogWidth = qMin(maximumWidth, parentWidget() ? parentWidget()->width() : width());

    resize(dialogWidth, qMax(160, sizeHint().height()));

    const QSize parentSize = parentWidget() ? parentWidget()->size() : size();
    move(parentSize.width() / 2 - this->width() / 2,
         parentSize.height() / 2 - this->height() / 2);
}

void QtMaterialDialog::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    if (d_ptr->scrim && parentWidget()) {
        d_ptr->scrim->setGeometry(parentWidget()->rect());
    }
}

void QtMaterialDialog::keyPressEvent(QKeyEvent* event)
{
    if (!event) {
        return;
    }

    if (event->key() == Qt::Key_Escape && d_ptr->dismissOnEscape) {
        event->accept();
        reject();
        return;
    }

    if ((event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
        && d_ptr->defaultButton && d_ptr->defaultButton->isEnabled() && d_ptr->defaultButton->isVisible()) {
        event->accept();
        d_ptr->defaultButton->click();
        return;
    }

    QtMaterialOverlaySurface::keyPressEvent(event);
}

bool QtMaterialDialog::focusNextPrevChild(bool next)
{
    if (!isVisible()) {
        return QtMaterialOverlaySurface::focusNextPrevChild(next);
    }

    return moveFocusInsideDialog(next);
}

void QtMaterialDialog::paintEvent(QPaintEvent*)
{
    resolveSpecIfNeeded();
    syncChildGeometry();

    const QRectF panel =
        QRectF(rect()).adjusted(2, 2, -2, -2);
    if (!panel.isValid()) {
        return;
    }

    const qreal radius =
        d_ptr->spec.cornerRadius < 0.0
        ? panel.height() / 2.0
        : qMin(
            d_ptr->spec.cornerRadius,
            panel.height() / 2.0);

    QPainterPath panelPath;
    panelPath.addRoundedRect(
        panel,
        radius,
        radius);

    QPainter painter(this);
    painter.setRenderHint(
        QPainter::Antialiasing,
        true);

    if (d_ptr->spec.hasResolvedElevationStyle) {
        QtMaterialElevationRenderer::paintPathElevation(
            &painter,
            panelPath,
            d_ptr->spec.shadowColor,
            d_ptr->spec.elevationStyle);
    }

    painter.setPen(Qt::NoPen);
    painter.setBrush(d_ptr->spec.containerColor);
    painter.drawPath(panelPath);
}

void QtMaterialDialog::updateAccessibilityMetadata()
{
    setAccessibleName(d_ptr->titleText.isEmpty() ? QStringLiteral("Dialog") : d_ptr->titleText);
    setAccessibleDescription(d_ptr->supportingText);

#if QT_CONFIG(accessibility)
    QAccessibleEvent event(this, QAccessible::NameChanged);
    QAccessible::updateAccessibility(&event);

    QAccessibleEvent descriptionEvent(this, QAccessible::DescriptionChanged);
    QAccessible::updateAccessibility(&descriptionEvent);
#endif
}

QList<QWidget*> QtMaterialDialog::focusableDialogChildren() const
{
    QList<QWidget*> result;

    const auto children = findChildren<QWidget*>(QString(), Qt::FindChildrenRecursively);
    result.reserve(children.size());

    for (QWidget* child : children) {
        if (!child || child == d_ptr->scrim) {
            continue;
        }

        if (!child->isEnabled() || !child->isVisibleTo(const_cast<QtMaterialDialog*>(this))) {
            continue;
        }

        if (child->focusPolicy() == Qt::NoFocus) {
            continue;
        }

        result.append(child);
    }

    if (result.isEmpty() && const_cast<QtMaterialDialog*>(this)->focusPolicy() != Qt::NoFocus) {
        result.append(const_cast<QtMaterialDialog*>(this));
    }

    std::sort(result.begin(), result.end(), [](QWidget* lhs, QWidget* rhs) {
        if (!lhs || !rhs || lhs->parentWidget() != rhs->parentWidget()) {
            return lhs < rhs;
        }

        const QPoint left = lhs->mapTo(lhs->window(), QPoint(0, 0));
        const QPoint right = rhs->mapTo(rhs->window(), QPoint(0, 0));

        if (left.y() == right.y()) {
            return left.x() < right.x();
        }

        return left.y() < right.y();
    });

    return result;
}

void QtMaterialDialog::focusInitialWidget()
{
    QWidget* target = nullptr;

    if (d_ptr->initialFocusWidget && d_ptr->initialFocusWidget->isEnabled()
        && d_ptr->initialFocusWidget->isVisibleTo(this)
        && d_ptr->initialFocusWidget->focusPolicy() != Qt::NoFocus) {
        target = d_ptr->initialFocusWidget;
    } else {
        const auto focusable = focusableDialogChildren();
        if (!focusable.isEmpty()) {
            target = focusable.first();
        }
    }

    if (!target) {
        target = this;
    }

    target->setFocus(Qt::OtherFocusReason);
}

bool QtMaterialDialog::moveFocusInsideDialog(bool next)
{
    const auto focusable = focusableDialogChildren();
    if (focusable.isEmpty()) {
        setFocus(next ? Qt::TabFocusReason : Qt::BacktabFocusReason);
        return true;
    }

    QWidget* current = QApplication::focusWidget();
    int currentIndex = focusable.indexOf(current);

    if (currentIndex < 0) {
        currentIndex = next ? -1 : 0;
    }

    const int direction = next ? 1 : -1;
    const int nextIndex = (currentIndex + direction + focusable.size()) % focusable.size();
    QWidget* target = focusable.at(nextIndex);

    if (target) {
        target->setFocus(next ? Qt::TabFocusReason : Qt::BacktabFocusReason);
    }

    return true;
}

} // namespace QtMaterial
