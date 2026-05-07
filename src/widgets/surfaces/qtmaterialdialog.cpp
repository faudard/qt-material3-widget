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
#include "qtmaterial/effects/qtmaterialshadowrenderer.h"
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"

namespace QtMaterial {

QtMaterialDialog::QtMaterialDialog(QWidget* parent)
    : QtMaterialOverlaySurface(parent)
    , m_layout(new QVBoxLayout(this))
    , m_scrim(new QtMaterialScrimWidget(parent ? parent : this))
    , m_transition(new QtMaterialTransitionController(this))
{
    setObjectName(QStringLiteral("qtmaterial_dialog"));
    setFocusPolicy(Qt::StrongFocus);
    setAccessibleName(QStringLiteral("Dialog"));

    m_layout->setContentsMargins(24, 24, 24, 24);

    hide();
    if (m_scrim) {
        m_scrim->hide();
    }

    updateAccessibilityMetadata();
}

QtMaterialDialog::~QtMaterialDialog() = default;

QString QtMaterialDialog::titleText() const
{
    return m_titleText;
}

void QtMaterialDialog::setTitleText(const QString& title)
{
    if (m_titleText == title) {
        return;
    }

    m_titleText = title;
    updateAccessibilityMetadata();
}

QString QtMaterialDialog::supportingText() const
{
    return m_supportingText;
}

void QtMaterialDialog::setSupportingText(const QString& text)
{
    if (m_supportingText == text) {
        return;
    }

    m_supportingText = text;
    updateAccessibilityMetadata();
}

void QtMaterialDialog::setBodyWidget(QWidget* widget)
{
    if (m_bodyWidget == widget) {
        return;
    }

    if (m_bodyWidget) {
        m_layout->removeWidget(m_bodyWidget);
        m_bodyWidget->setParent(nullptr);
    }

    m_bodyWidget = widget;

    if (m_bodyWidget) {
        m_bodyWidget->setParent(this);
        m_layout->addWidget(m_bodyWidget);

        if (!m_initialFocusWidget) {
            m_initialFocusWidget = m_bodyWidget;
        }
    }

    updateAccessibilityMetadata();
}

QWidget* QtMaterialDialog::bodyWidget() const
{
    return m_bodyWidget;
}

QWidget* QtMaterialDialog::initialFocusWidget() const
{
    return m_initialFocusWidget;
}

void QtMaterialDialog::setInitialFocusWidget(QWidget* widget)
{
    if (m_initialFocusWidget == widget) {
        return;
    }

    m_initialFocusWidget = widget;
}

QAbstractButton* QtMaterialDialog::defaultButton() const
{
    return m_defaultButton;
}

void QtMaterialDialog::setDefaultButton(QAbstractButton* button)
{
    if (m_defaultButton == button) {
        return;
    }

    m_defaultButton = button;
}

bool QtMaterialDialog::dismissOnEscape() const noexcept
{
    return m_dismissOnEscape;
}

void QtMaterialDialog::setDismissOnEscape(bool enabled)
{
    m_dismissOnEscape = enabled;
}

bool QtMaterialDialog::restoreFocusOnClose() const noexcept
{
    return m_restoreFocusOnClose;
}

void QtMaterialDialog::setRestoreFocusOnClose(bool enabled)
{
    m_restoreFocusOnClose = enabled;
}

QString QtMaterialDialog::accessibilitySummary() const
{
    if (!m_titleText.isEmpty() && !m_supportingText.isEmpty()) {
        return m_titleText + QStringLiteral(". ") + m_supportingText;
    }

    if (!m_titleText.isEmpty()) {
        return m_titleText;
    }

    if (!m_supportingText.isEmpty()) {
        return m_supportingText;
    }

    return QStringLiteral("Dialog");
}

void QtMaterialDialog::open()
{
    m_previousFocusWidget = QApplication::focusWidget();

    resolveSpecIfNeeded();
    syncChildGeometry();

    if (m_scrim) {
        m_scrim->setGeometry(parentWidget() ? parentWidget()->rect() : rect());
        m_scrim->show();
        m_scrim->raise();
    }

    show();
    raise();
    activateWindow();

    if (m_transition) {
        m_transition->startForward();
    }

    focusInitialWidget();
    Q_EMIT opened();
}

void QtMaterialDialog::close()
{
    if (m_transition) {
        m_transition->startBackward();
    }

    hide();

    if (m_scrim) {
        m_scrim->hide();
    }

    if (m_restoreFocusOnClose && m_previousFocusWidget) {
        m_previousFocusWidget->setFocus(Qt::OtherFocusReason);
    }

    Q_EMIT closed();
}

void QtMaterialDialog::reject()
{
    close();
    Q_EMIT rejected();
}

void QtMaterialDialog::themeChangedEvent(const Theme& theme)
{
    QtMaterialOverlaySurface::themeChangedEvent(theme);
    m_specDirty = true;
}

void QtMaterialDialog::invalidateResolvedSpec()
{
    m_specDirty = true;
}

void QtMaterialDialog::resolveSpecIfNeeded() const
{
    if (!m_specDirty) {
        return;
    }

    SpecFactory factory;
    m_spec = factory.dialogSpec(theme());
    m_specDirty = false;
}

void QtMaterialDialog::syncChildGeometry()
{
    resolveSpecIfNeeded();

    const int maximumWidth = qMax(1, m_spec.maxWidth);
    const int dialogWidth = qMin(maximumWidth, parentWidget() ? parentWidget()->width() : width());

    resize(dialogWidth, qMax(160, sizeHint().height()));

    const QSize parentSize = parentWidget() ? parentWidget()->size() : size();
    move(parentSize.width() / 2 - this->width() / 2,
         parentSize.height() / 2 - this->height() / 2);
}

void QtMaterialDialog::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    if (m_scrim && parentWidget()) {
        m_scrim->setGeometry(parentWidget()->rect());
    }
}

void QtMaterialDialog::keyPressEvent(QKeyEvent* event)
{
    if (!event) {
        return;
    }

    if (event->key() == Qt::Key_Escape && m_dismissOnEscape) {
        event->accept();
        reject();
        return;
    }

    if ((event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
        && m_defaultButton && m_defaultButton->isEnabled() && m_defaultButton->isVisible()) {
        event->accept();
        m_defaultButton->click();
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

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QRectF panel = rect().adjusted(2, 2, -2, -2);

    QtMaterialShadowRenderer::paintRoundedShadow(&painter,
                                                 panel,
                                                 28.0,
                                                 QColor(0, 0, 0, 80),
                                                 12,
                                                 4);

    painter.setPen(Qt::NoPen);
    painter.setBrush(m_spec.containerColor);
    painter.drawRoundedRect(panel, 28.0, 28.0);
}

void QtMaterialDialog::updateAccessibilityMetadata()
{
    setAccessibleName(m_titleText.isEmpty() ? QStringLiteral("Dialog") : m_titleText);
    setAccessibleDescription(m_supportingText);

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
        if (!child || child == m_scrim) {
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

    if (m_initialFocusWidget && m_initialFocusWidget->isEnabled()
        && m_initialFocusWidget->isVisibleTo(this)
        && m_initialFocusWidget->focusPolicy() != Qt::NoFocus) {
        target = m_initialFocusWidget;
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
