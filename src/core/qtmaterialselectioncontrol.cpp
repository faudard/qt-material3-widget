#include "qtmaterial/core/qtmaterialselectioncontrol.h"

#include <QFontMetrics>
#include <QtGlobal>

namespace {

constexpr int kIndicatorSize = 18;
constexpr int kTouchTargetSize = 48;
constexpr int kHorizontalPadding = 16;
constexpr int kMinimumControlHeight = 48;

} // namespace

namespace QtMaterial {

QtMaterialSelectionControl::QtMaterialSelectionControl(QWidget* parent)
    : QtMaterialAbstractButton(parent)
    , m_labelAlignment(Qt::AlignLeft | Qt::AlignVCenter)
    , m_spacing(12)
{
    setCheckable(true);
}

QtMaterialSelectionControl::~QtMaterialSelectionControl() = default;

Qt::Alignment QtMaterialSelectionControl::labelAlignment() const noexcept
{
    return m_labelAlignment;
}

void QtMaterialSelectionControl::setLabelAlignment(Qt::Alignment alignment)
{
    if (m_labelAlignment == alignment) {
        return;
    }

    m_labelAlignment = alignment;
    updateGeometry();
    update();
}

int QtMaterialSelectionControl::spacing() const noexcept
{
    return m_spacing;
}

void QtMaterialSelectionControl::setSpacing(int spacing)
{
    const int boundedSpacing = qMax(0, spacing);

    if (m_spacing == boundedSpacing) {
        return;
    }

    m_spacing = boundedSpacing;
    updateGeometry();
    update();
}

QRect QtMaterialSelectionControl::indicatorRect() const
{
    const int touchWidth = qMin(width(), kTouchTargetSize);
    const int xInsideTouchTarget = qMax(0, (touchWidth - kIndicatorSize) / 2);
    const int y = qMax(0, (height() - kIndicatorSize) / 2);

    const bool rtl = layoutDirection() == Qt::RightToLeft;

    const int x = rtl
                      ? qMax(0, width() - touchWidth + xInsideTouchTarget)
                      : xInsideTouchTarget;

    return QRect(x, y, kIndicatorSize, kIndicatorSize);
}

QRect QtMaterialSelectionControl::labelRect() const
{
    const bool hasText = !text().isEmpty();
    const int gap = hasText ? m_spacing : 0;
    const int touchWidth = qMin(width(), kTouchTargetSize);

    const bool rtl = layoutDirection() == Qt::RightToLeft;

    if (rtl) {
        return QRect(
            0,
            0,
            qMax(0, width() - touchWidth - gap),
            height()
            );
    }

    const int x = touchWidth + gap;

    return QRect(
        x,
        0,
        qMax(0, width() - x),
        height()
        );
}

QSize QtMaterialSelectionControl::sizeHint() const
{
    const QFontMetrics fm(font());

    const bool hasText = !text().isEmpty();
    const int textWidth = hasText ? fm.horizontalAdvance(text()) : 0;
    const int gap = hasText ? m_spacing : 0;

    return QSize(
        kTouchTargetSize + gap + textWidth + kHorizontalPadding,
        qMax(kMinimumControlHeight, fm.height() + kHorizontalPadding)
        );
}

} // namespace QtMaterial