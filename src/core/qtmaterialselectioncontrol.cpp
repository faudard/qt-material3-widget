#include "qtmaterial/core/qtmaterialselectioncontrol.h"

#include <QFontMetrics>
#include <QtGlobal>

namespace {

constexpr int kIndicatorSize = 18;
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
    const int y = (height() - kIndicatorSize) / 2;
    const bool rtl = (layoutDirection() == Qt::RightToLeft);
    const int x = rtl ? qMax(0, width() - kIndicatorSize) : 0;

    return QRect(x, y, kIndicatorSize, kIndicatorSize);
}

QRect QtMaterialSelectionControl::labelRect() const
{
    const int labelWidth = qMax(0, width() - kIndicatorSize - m_spacing);
    const bool rtl = (layoutDirection() == Qt::RightToLeft);
    const int x = rtl ? 0 : (kIndicatorSize + m_spacing);

    return QRect(x, 0, labelWidth, height());
}

QSize QtMaterialSelectionControl::sizeHint() const
{
    const QFontMetrics fm(font());
    const int textWidth = fm.horizontalAdvance(text());

    return QSize(kIndicatorSize + m_spacing + textWidth + kHorizontalPadding,
                 qMax(kMinimumControlHeight, fm.height() + kHorizontalPadding));
}

} // namespace QtMaterial