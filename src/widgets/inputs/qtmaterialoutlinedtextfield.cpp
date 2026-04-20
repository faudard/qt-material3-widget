#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"
#include <QLineEdit>
#include <QPainter>
#include <QResizeEvent>
#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
namespace QtMaterial {
QtMaterialOutlinedTextField::QtMaterialOutlinedTextField(QWidget* parent)
    : QtMaterialInputControl(parent)
    , m_lineEdit(new QLineEdit(this))
    , m_transition(new QtMaterialTransitionController(this))
{
    setMinimumHeight(64);
}
QtMaterialOutlinedTextField::~QtMaterialOutlinedTextField() = default;
QString QtMaterialOutlinedTextField::text() const { return m_lineEdit ? m_lineEdit->text() : QString(); }
void QtMaterialOutlinedTextField::setText(const QString& text) { if (m_lineEdit) m_lineEdit->setText(text); }
QLineEdit* QtMaterialOutlinedTextField::lineEdit() const { return m_lineEdit; }
void QtMaterialOutlinedTextField::themeChangedEvent(const Theme& theme) { QtMaterialInputControl::themeChangedEvent(theme); m_specDirty = true; }
void QtMaterialOutlinedTextField::invalidateResolvedSpec() { m_specDirty = true; }
void QtMaterialOutlinedTextField::resolveSpecIfNeeded() const
{
    if (!m_specDirty) return;
    SpecFactory factory;
    m_spec = factory.outlinedTextFieldSpec(theme(), density());
    m_specDirty = false;
}
void QtMaterialOutlinedTextField::resizeEvent(QResizeEvent* event)
{
    QtMaterialInputControl::resizeEvent(event);
    if (m_lineEdit) m_lineEdit->setGeometry(contentRect());
}
void QtMaterialOutlinedTextField::paintEvent(QPaintEvent*)
{
    resolveSpecIfNeeded();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QRectF box = rect().adjusted(1, 1, -1, -24);
    const QColor outline = hasErrorState() ? m_spec.errorColor : (interactionState().isFocused() ? m_spec.focusedOutlineColor : m_spec.outlineColor);
    painter.setPen(QPen(outline, interactionState().isFocused() ? 2.0 : 1.0));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(box, 4.0, 4.0);
    painter.setPen(hasErrorState() ? m_spec.errorColor : m_spec.labelColor);
    painter.drawText(QRect(12, 0, width() - 24, 20), Qt::AlignLeft | Qt::AlignVCenter, labelText());
    painter.setPen(hasErrorState() ? m_spec.errorColor : m_spec.supportingTextColor);
    painter.drawText(supportingTextRect(), Qt::AlignLeft | Qt::AlignVCenter, hasErrorState() ? errorText() : supportingText());
    if (interactionState().isFocused()) {
        QtMaterialFocusIndicator::paintRectFocusRing(&painter, box.adjusted(-2, -2, 2, 2), m_spec.focusedOutlineColor, 6.0, 1.5);
    }
}
} // namespace QtMaterial
