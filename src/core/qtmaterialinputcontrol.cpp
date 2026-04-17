#include "qtmaterial/core/qtmaterialinputcontrol.h"
namespace QtMaterial {
QtMaterialInputControl::QtMaterialInputControl(QWidget* parent) : QtMaterialControl(parent), m_labelText(), m_supportingText(), m_errorText(), m_hasErrorState(false) {}
QtMaterialInputControl::~QtMaterialInputControl() = default;
QString QtMaterialInputControl::labelText() const { return m_labelText; }
void QtMaterialInputControl::setLabelText(const QString& text) { if (m_labelText == text) return; m_labelText = text; updateGeometry(); update(); }
QString QtMaterialInputControl::supportingText() const { return m_supportingText; }
void QtMaterialInputControl::setSupportingText(const QString& text) { if (m_supportingText == text) return; m_supportingText = text; updateGeometry(); update(); }
QString QtMaterialInputControl::errorText() const { return m_errorText; }
void QtMaterialInputControl::setErrorText(const QString& text) { if (m_errorText == text) return; m_errorText = text; update(); }
bool QtMaterialInputControl::hasErrorState() const noexcept { return m_hasErrorState; }
void QtMaterialInputControl::setHasErrorState(bool value) { if (m_hasErrorState == value) return; m_hasErrorState = value; interactionState().setError(value); stateChangedEvent(); }
QRect QtMaterialInputControl::contentRect() const { return rect().adjusted(16, 8, -16, -24); }
QRect QtMaterialInputControl::supportingTextRect() const { return QRect(16, height() - 20, width() - 32, 16); }
} // namespace QtMaterial
