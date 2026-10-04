#include "qtmaterial/widgets/surfaces/qtmaterialsidesheet.h"

#include <QApplication>
#include <QEvent>
#include <QHBoxLayout>
#include <QHideEvent>
#include <QKeyEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPointer>
#include <QShowEvent>
#include <QToolButton>
#include <QVBoxLayout>

#include <algorithm>

#include "qtmaterial/effects/qtmaterialscrimwidget.h"
#include "../resolution/qtmaterialmissingmaterial3specresolution_p.h"

namespace QtMaterial {

namespace {

constexpr int kPreferredWidth = 360;
constexpr int kMinimumWidth = 256;
constexpr int kCornerRadius = 28;

} // namespace

class QtMaterialSideSheetPrivate
{
public:
    QtMaterialSideSheet::Edge edge = QtMaterialSideSheet::Edge::Right;
    bool modal = true;
    bool dismissOnScrimClick = true;
    bool restoreFocusOnClose = true;
    bool open = false;
    QString titleText;

    QPointer<QWidget> initialFocusWidget;
    QPointer<QWidget> lastFocusBeforeOpen;
    QLabel* titleLabel = nullptr;
    QToolButton* closeButton = nullptr;
    QWidget* content = nullptr;
    QPointer<QtMaterialScrimWidget> scrim;
};

QtMaterialSideSheet::QtMaterialSideSheet(QWidget* parent)
    : QtMaterialOverlaySurface(parent)
    , d_ptr(std::make_unique<QtMaterialSideSheetPrivate>())
{
    if (parent) {
        setHostWidget(parent);
    }

    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_StyledBackground, false);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    setAccessibleName(tr("Side sheet"));

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(24, 16, 24, 24);
    root->setSpacing(12);

    auto* header = new QHBoxLayout;
    header->setSpacing(8);

    d_ptr->titleLabel = new QLabel(this);
    QFont titleFont = d_ptr->titleLabel->font();
    titleFont.setBold(true);
    d_ptr->titleLabel->setFont(titleFont);
    header->addWidget(d_ptr->titleLabel, 1);

    d_ptr->closeButton = new QToolButton(this);
    d_ptr->closeButton->setText(QStringLiteral("×"));
    d_ptr->closeButton->setAccessibleName(tr("Close side sheet"));
    d_ptr->closeButton->setToolTip(tr("Close"));
    header->addWidget(d_ptr->closeButton);

    connect(
        d_ptr->closeButton,
        &QToolButton::clicked,
        this,
        &QtMaterialSideSheet::closeSheet);

    d_ptr->content = new QWidget(this);
    d_ptr->content->setObjectName(QStringLiteral("qtmaterial_side_sheet_content"));
    d_ptr->content->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    root->addLayout(header);
    root->addWidget(d_ptr->content, 1);

    hide();
    syncAccessibility();
}

QtMaterialSideSheet::~QtMaterialSideSheet()
{
    if (d_ptr->scrim) {
        d_ptr->scrim->removeEventFilter(this);
        d_ptr->scrim->deleteLater();
    }
}

QtMaterialSideSheet::Edge QtMaterialSideSheet::edge() const noexcept
{
    return d_ptr->edge;
}

void QtMaterialSideSheet::setEdge(Edge edge)
{
    if (d_ptr->edge == edge) {
        return;
    }

    d_ptr->edge = edge;
    syncGeometryToHost();
    update();
    Q_EMIT edgeChanged(edge);
}

bool QtMaterialSideSheet::isModal() const noexcept
{
    return d_ptr->modal;
}

void QtMaterialSideSheet::setModal(bool modal)
{
    if (d_ptr->modal == modal) {
        return;
    }

    d_ptr->modal = modal;
    syncScrim();
    Q_EMIT modalChanged(modal);
}

QString QtMaterialSideSheet::titleText() const
{
    return d_ptr->titleText;
}

void QtMaterialSideSheet::setTitleText(const QString& text)
{
    if (d_ptr->titleText == text) {
        return;
    }

    d_ptr->titleText = text;
    d_ptr->titleLabel->setText(text);
    syncAccessibility();
    Q_EMIT titleTextChanged(text);
}

bool QtMaterialSideSheet::dismissOnScrimClick() const noexcept
{
    return d_ptr->dismissOnScrimClick;
}

void QtMaterialSideSheet::setDismissOnScrimClick(bool enabled)
{
    if (d_ptr->dismissOnScrimClick == enabled) {
        return;
    }

    d_ptr->dismissOnScrimClick = enabled;
    Q_EMIT dismissOnScrimClickChanged(enabled);
}

bool QtMaterialSideSheet::restoreFocusOnClose() const noexcept
{
    return d_ptr->restoreFocusOnClose;
}

void QtMaterialSideSheet::setRestoreFocusOnClose(bool enabled)
{
    if (d_ptr->restoreFocusOnClose == enabled) {
        return;
    }

    d_ptr->restoreFocusOnClose = enabled;
    Q_EMIT restoreFocusOnCloseChanged(enabled);
}

QWidget* QtMaterialSideSheet::initialFocusWidget() const noexcept
{
    return d_ptr->initialFocusWidget.data();
}

void QtMaterialSideSheet::setInitialFocusWidget(QWidget* widget)
{
    if (widget && widget != this && !isAncestorOf(widget)) {
        return;
    }
    d_ptr->initialFocusWidget = widget;
}

QWidget* QtMaterialSideSheet::contentWidget() const noexcept
{
    return d_ptr->content;
}

bool QtMaterialSideSheet::isOpen() const noexcept
{
    return d_ptr->open;
}

QSize QtMaterialSideSheet::sizeHint() const
{
    const QWidget* host = hostWidget() ? hostWidget() : parentWidget();
    return QSize(
        kPreferredWidth,
        host ? qMax(240, host->height()) : 640);
}

QSize QtMaterialSideSheet::minimumSizeHint() const
{
    return QSize(kMinimumWidth, 240);
}

void QtMaterialSideSheet::open()
{
    if (!isVisible() && !d_ptr->open) {
        d_ptr->lastFocusBeforeOpen = QApplication::focusWidget();
    }

    syncGeometryToHost();
    show();
    raise();
    syncScrim();
    focusFirstChild();
}

void QtMaterialSideSheet::closeSheet()
{
    if (!isVisible() && !d_ptr->open) {
        return;
    }

    hide();
    syncScrim();
    restorePreviousFocus();
    Q_EMIT dismissed();
}

void QtMaterialSideSheet::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)

    const QRectF bounds(rect());
    if (!bounds.isValid()) {
        return;
    }

    const qreal radius = qMin<qreal>(
        kCornerRadius,
        qMin(bounds.width(), bounds.height()) / 2.0);

    QPainterPath path;
    if (d_ptr->edge == Edge::Right) {
        path.moveTo(bounds.right(), bounds.top());
        path.lineTo(bounds.left() + radius, bounds.top());
        path.quadTo(bounds.left(), bounds.top(), bounds.left(), bounds.top() + radius);
        path.lineTo(bounds.left(), bounds.bottom() - radius);
        path.quadTo(bounds.left(), bounds.bottom(), bounds.left() + radius, bounds.bottom());
        path.lineTo(bounds.right(), bounds.bottom());
    } else {
        path.moveTo(bounds.left(), bounds.top());
        path.lineTo(bounds.right() - radius, bounds.top());
        path.quadTo(bounds.right(), bounds.top(), bounds.right(), bounds.top() + radius);
        path.lineTo(bounds.right(), bounds.bottom() - radius);
        path.quadTo(bounds.right(), bounds.bottom(), bounds.right() - radius, bounds.bottom());
        path.lineTo(bounds.left(), bounds.bottom());
    }
    path.closeSubpath();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    const auto spec =
        MissingMaterial3SpecResolution::sideSheetSpec(
            theme(),
            palette());

    painter.setPen(Qt::NoPen);
    painter.setBrush(spec.containerColor);
    painter.drawPath(path);
}

void QtMaterialSideSheet::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        closeSheet();
        event->accept();
        return;
    }
    QtMaterialOverlaySurface::keyPressEvent(event);
}

bool QtMaterialSideSheet::focusNextPrevChild(bool next)
{
    if (!d_ptr->modal || !isVisible() || !d_ptr->open) {
        return QtMaterialOverlaySurface::focusNextPrevChild(next);
    }
    return moveFocusInsideSheet(next);
}

void QtMaterialSideSheet::showEvent(QShowEvent* event)
{
    QtMaterialOverlaySurface::showEvent(event);
    if (!d_ptr->open) {
        d_ptr->open = true;
        Q_EMIT openChanged(true);
    }
    syncGeometryToHost();
    syncScrim();
}

void QtMaterialSideSheet::hideEvent(QHideEvent* event)
{
    QtMaterialOverlaySurface::hideEvent(event);
    if (d_ptr->open) {
        d_ptr->open = false;
        Q_EMIT openChanged(false);
    }
    syncScrim();
}

bool QtMaterialSideSheet::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == d_ptr->scrim
        && event->type() == QEvent::MouseButtonPress
        && d_ptr->dismissOnScrimClick) {
        auto* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            closeSheet();
            return true;
        }
    }

    return QtMaterialOverlaySurface::eventFilter(watched, event);
}

void QtMaterialSideSheet::syncGeometryToHost()
{
    QWidget* host = hostWidget() ? hostWidget() : parentWidget();
    if (!host) {
        return;
    }

    const int preferred = qMin(kPreferredWidth, host->width());
    const int width = qMin(
        host->width(),
        qMax(qMin(kMinimumWidth, host->width()), preferred));
    const int x = d_ptr->edge == Edge::Left ? 0 : host->width() - width;
    setGeometry(x, 0, width, host->height());

    if (d_ptr->scrim) {
        d_ptr->scrim->setGeometry(host->rect());
    }
}

void QtMaterialSideSheet::themeChangedEvent(const QtMaterial::Theme& theme)
{
    QtMaterialOverlaySurface::themeChangedEvent(theme);
    update();
}

void QtMaterialSideSheet::focusFirstChild()
{
    QWidget* target = d_ptr->initialFocusWidget.data();
    if (!target
        || (target != this && !isAncestorOf(target))
        || !target->isVisible()
        || !target->isEnabled()
        || target->focusPolicy() == Qt::NoFocus) {
        const QList<QWidget*> children = focusableSheetChildren();
        target = children.isEmpty() ? static_cast<QWidget*>(this) : children.first();
    }

    target->setFocus(Qt::OtherFocusReason);
}

QList<QWidget*> QtMaterialSideSheet::focusableSheetChildren() const
{
    QList<QWidget*> result;

    const auto appendIfFocusable = [&result, this](QWidget* child) {
        if (!child
            || !child->isEnabled()
            || !child->isVisibleTo(this)
            || child->focusPolicy() == Qt::NoFocus) {
            return;
        }
        if (!result.contains(child)) {
            result.append(child);
        }
    };

    appendIfFocusable(d_ptr->closeButton);

    const auto children = d_ptr->content->findChildren<QWidget*>(
        QString(),
        Qt::FindChildrenRecursively);
    for (QWidget* child : children) {
        appendIfFocusable(child);
    }

    std::sort(
        result.begin(),
        result.end(),
        [](QWidget* lhs, QWidget* rhs) {
            const QPoint left = lhs->mapTo(lhs->window(), QPoint(0, 0));
            const QPoint right = rhs->mapTo(rhs->window(), QPoint(0, 0));
            if (left.y() == right.y()) {
                return left.x() < right.x();
            }
            return left.y() < right.y();
        });

    return result;
}

bool QtMaterialSideSheet::moveFocusInsideSheet(bool next)
{
    const QList<QWidget*> focusable = focusableSheetChildren();
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
    const int nextIndex =
        (currentIndex + direction + focusable.size())
        % focusable.size();
    focusable.at(nextIndex)->setFocus(
        next ? Qt::TabFocusReason : Qt::BacktabFocusReason);
    return true;
}

void QtMaterialSideSheet::restorePreviousFocus()
{
    if (!d_ptr->restoreFocusOnClose) {
        d_ptr->lastFocusBeforeOpen.clear();
        return;
    }

    QWidget* target = d_ptr->lastFocusBeforeOpen.data();
    d_ptr->lastFocusBeforeOpen.clear();
    if (!target
        || !target->isVisible()
        || !target->isEnabled()
        || target->focusPolicy() == Qt::NoFocus) {
        return;
    }
    target->setFocus(Qt::OtherFocusReason);
}

void QtMaterialSideSheet::syncScrim()
{
    QWidget* host = hostWidget() ? hostWidget() : parentWidget();

    if (!d_ptr->modal || !d_ptr->open || !host) {
        if (d_ptr->scrim) {
            d_ptr->scrim->hide();
        }
        return;
    }

    if (!d_ptr->scrim || d_ptr->scrim->parentWidget() != host) {
        if (d_ptr->scrim) {
            d_ptr->scrim->removeEventFilter(this);
            d_ptr->scrim->deleteLater();
        }

        d_ptr->scrim = new QtMaterialScrimWidget(host);
        d_ptr->scrim->setObjectName(QStringLiteral("qtmaterial_side_sheet_scrim"));
        d_ptr->scrim->installEventFilter(this);
    }

    d_ptr->scrim->setGeometry(host->rect());
    d_ptr->scrim->show();
    d_ptr->scrim->raise();
    raise();
}

void QtMaterialSideSheet::syncAccessibility()
{
    const QString description = d_ptr->titleText.isEmpty()
        ? tr("Side sheet")
        : tr("Side sheet: %1").arg(d_ptr->titleText);

    if (accessibleName().isEmpty() || accessibleName() == tr("Side sheet")) {
        setAccessibleName(d_ptr->titleText.isEmpty() ? tr("Side sheet") : d_ptr->titleText);
    }
    setAccessibleDescription(description);
}

} // namespace QtMaterial
