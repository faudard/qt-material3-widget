#include "qtmaterial/core/qtmaterialselectioncontrol.h"
#include <QFontMetrics>
namespace QtMaterial {
QtMaterialSelectionControl::QtMaterialSelectionControl(QWidget* parent) : QtMaterialAbstractButton(parent), m_labelAlignment(Qt::AlignLeft | Qt::AlignVCenter), m_spacing(12) { setCheckable(true); }
QtMaterialSelectionControl::~QtMaterialSelectionControl() = default;
Qt::Alignment QtMaterialSelectionControl::labelAlignment() const noexcept { return m_labelAlignment; }
void QtMaterialSelectionControl::setLabelAlignment(Qt::Alignment alignment) { if (m_labelAlignment == alignment) return; m_labelAlignment = alignment; updateGeometry(); update(); }
int QtMaterialSelectionControl::spacing() const noexcept { return m_spacing; }
void QtMaterialSelectionControl::setSpacing(int spacing) { if (m_spacing == spacing) return; m_spacing = spacing; updateGeometry(); update(); }
QRect QtMaterialSelectionControl::indicatorRect() const { const int indicatorSize = 18; const int y = (height() - indicatorSize) / 2; return QRect(0, y, indicatorSize, indicatorSize); }
QRect QtMaterialSelectionControl::labelRect() const { const QRect ind = indicatorRect(); return QRect(ind.right() + 1 + m_spacing, 0, width() - ind.width() - m_spacing, height()); }
QSize QtMaterialSelectionControl::sizeHint() const { const QFontMetrics fm(font()); return QSize(18 + m_spacing + fm.horizontalAdvance(text()) + 16, qMax(48, fm.height() + 16)); }
} // namespace QtMaterial
