#include "qtmaterial/widgets/data/qtmateriallistitem.h"

#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QResizeEvent>
#include <memory>

namespace QtMaterial {

struct QtMaterialListItemPrivate
{
    mutable bool m_specDirty = true;
    mutable bool m_layoutDirty = true;
    mutable QtMaterial::ListItemSpec m_spec;
    QString m_headlineText;
    QString m_supportingText;
    QIcon m_leadingIcon;
    QIcon m_trailingIcon;
    bool m_selected = false;
    bool m_dividerVisible = false;
    QtMaterialListItem::DensityVariant m_densityVariant = QtMaterialListItem::DensityVariant::Standard;
};



namespace {
int heightForDensity(QtMaterialListItem::DensityVariant variant)
{
    switch (variant) {
    case QtMaterialListItem::DensityVariant::Compact:
        return 48;
    case QtMaterialListItem::DensityVariant::Large:
        return 72;
    case QtMaterialListItem::DensityVariant::Standard:
    default:
        return 56;
    }
}
}

QtMaterialListItem::QtMaterialListItem(QWidget* parent)
    : QtMaterialControl(parent)
{
    d_ptr = std::make_unique<QtMaterialListItemPrivate>();

    setFocusPolicy(Qt::StrongFocus);
    setMinimumHeight(heightForDensity(d_ptr->m_densityVariant));
}

QtMaterialListItem::QtMaterialListItem(const QString& headline, QWidget* parent)
    : QtMaterialControl(parent)
    , d_ptr->m_headlineText(headline)
{
    d_ptr = std::make_unique<QtMaterialListItemPrivate>();

    setFocusPolicy(Qt::StrongFocus);
    setMinimumHeight(heightForDensity(d_ptr->m_densityVariant));
}

QtMaterialListItem::~QtMaterialListItem() = default;

QString QtMaterialListItem::headlineText() const
{
    return d_ptr->m_headlineText;
}

void QtMaterialListItem::setHeadlineText(const QString& text)
{
    if (d_ptr->m_headlineText == text) {
        return;
    }

    d_ptr->m_headlineText = text;
    contentChangedEvent();
}

QString QtMaterialListItem::supportingText() const
{
    return d_ptr->m_supportingText;
}

void QtMaterialListItem::setSupportingText(const QString& text)
{
    if (d_ptr->m_supportingText == text) {
        return;
    }

    d_ptr->m_supportingText = text;
    contentChangedEvent();
}

QIcon QtMaterialListItem::leadingIcon() const
{
    return d_ptr->m_leadingIcon;
}

void QtMaterialListItem::setLeadingIcon(const QIcon& icon)
{
    d_ptr->m_leadingIcon = icon;
    update();
}

QIcon QtMaterialListItem::trailingIcon() const
{
    return d_ptr->m_trailingIcon;
}

void QtMaterialListItem::setTrailingIcon(const QIcon& icon)
{
    d_ptr->m_trailingIcon = icon;
    update();
}

bool QtMaterialListItem::isSelected() const noexcept
{
    return d_ptr->m_selected;
}

void QtMaterialListItem::setSelected(bool selected)
{
    if (d_ptr->m_selected == selected) {
        return;
    }

    d_ptr->m_selected = selected;
    emit selectionChanged(d_ptr->m_selected);
    update();
}

bool QtMaterialListItem::isDividerVisible() const noexcept
{
    return d_ptr->m_dividerVisible;
}

void QtMaterialListItem::setDividerVisible(bool visible)
{
    if (d_ptr->m_dividerVisible == visible) {
        return;
    }

    d_ptr->m_dividerVisible = visible;
    update();
}

QtMaterialListItem::DensityVariant QtMaterialListItem::densityVariant() const noexcept
{
    return d_ptr->m_densityVariant;
}

void QtMaterialListItem::setDensityVariant(DensityVariant variant)
{
    if (d_ptr->m_densityVariant == variant) {
        return;
    }

    d_ptr->m_densityVariant = variant;
    updateGeometry();
    setMinimumHeight(heightForDensity(d_ptr->m_densityVariant));
    update();
}

QSize QtMaterialListItem::sizeHint() const
{
    return QSize(240, heightForDensity(d_ptr->m_densityVariant));
}

QSize QtMaterialListItem::minimumSizeHint() const
{
    return QSize(120, heightForDensity(d_ptr->m_densityVariant));
}

void QtMaterialListItem::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    if (d_ptr->m_selected) {
        painter.fillRect(rect(), palette().alternateBase());
    }

    const int leftPadding = 16;
    const int iconSize = 20;
    int textLeft = leftPadding;

    if (!d_ptr->m_leadingIcon.isNull()) {
        d_ptr->m_leadingIcon.paint(&painter, QRect(leftPadding, (height() - iconSize) / 2, iconSize, iconSize));
        textLeft += iconSize + 12;
    }

    int textRight = width() - 16;
    if (!d_ptr->m_trailingIcon.isNull()) {
        const QRect trailingRect(width() - 16 - iconSize, (height() - iconSize) / 2, iconSize, iconSize);
        d_ptr->m_trailingIcon.paint(&painter, trailingRect);
        textRight = trailingRect.left() - 12;
    }

    painter.setPen(palette().windowText().color());

    const bool hasSupporting = !d_ptr->m_supportingText.isEmpty();
    if (hasSupporting) {
        painter.drawText(QRect(textLeft, 10, textRight - textLeft, 20),
                         Qt::AlignLeft | Qt::AlignVCenter,
                         d_ptr->m_headlineText.isEmpty() ? QStringLiteral("List item") : d_ptr->m_headlineText);

        painter.setPen(palette().mid().color());
        painter.drawText(QRect(textLeft, 30, textRight - textLeft, 18),
                         Qt::AlignLeft | Qt::AlignVCenter,
                         d_ptr->m_supportingText);
    } else {
        painter.drawText(QRect(textLeft, 0, textRight - textLeft, height()),
                         Qt::AlignLeft | Qt::AlignVCenter,
                         d_ptr->m_headlineText.isEmpty() ? QStringLiteral("List item") : d_ptr->m_headlineText);
    }

    if (d_ptr->m_dividerVisible) {
        painter.fillRect(QRect(16, height() - 1, qMax(0, width() - 32), 1), palette().mid());
    }
}

void QtMaterialListItem::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && rect().contains(event->pos())) {
        emit clicked();
        emit activated();
    }
    QtMaterialControl::mouseReleaseEvent(event);
}

void QtMaterialListItem::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Space:
        emit clicked();
        emit activated();
        event->accept();
        return;
    default:
        break;
    }

    QtMaterialControl::keyPressEvent(event);
}

void QtMaterialListItem::resizeEvent(QResizeEvent* event)
{
    QtMaterialControl::resizeEvent(event);
    d_ptr->m_layoutDirty = true;
}

void QtMaterialListItem::changeEvent(QEvent* event)
{
    QtMaterialControl::changeEvent(event);
    update();
}

void QtMaterialListItem::themeChangedEvent(const QtMaterial::Theme& theme)
{
    QtMaterialControl::themeChangedEvent(theme);
    d_ptr->m_specDirty = true;
    update();
}

void QtMaterialListItem::stateChangedEvent()
{
    QtMaterialControl::stateChangedEvent();
    update();
}

void QtMaterialListItem::contentChangedEvent()
{
    d_ptr->m_layoutDirty = true;
    updateGeometry();
    update();
}

void QtMaterialListItem::ensureSpecResolved() const
{
    d_ptr->m_specDirty = false;
}

void QtMaterialListItem::ensureLayoutResolved() const
{
    d_ptr->m_layoutDirty = false;
}

void QtMaterialListItem::invalidateLayoutCache()
{
    d_ptr->m_layoutDirty = true;
}

} // namespace QtMaterial