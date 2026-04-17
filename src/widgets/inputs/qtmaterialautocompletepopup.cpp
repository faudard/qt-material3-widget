#include "qtmaterial/widgets/inputs/qtmaterialautocompletepopup.h"
#include <QPainter>
namespace QtMaterial {
QtMaterialAutocompletePopup::QtMaterialAutocompletePopup(QWidget* parent) : QWidget(parent) { setMinimumSize(200, 120); }
QtMaterialAutocompletePopup::~QtMaterialAutocompletePopup() = default;
void QtMaterialAutocompletePopup::setSuggestions(const QStringList& suggestions) { m_suggestions = suggestions; update(); }
QStringList QtMaterialAutocompletePopup::suggestions() const { return m_suggestions; }
void QtMaterialAutocompletePopup::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.fillRect(rect(), palette().base());
    painter.setPen(palette().windowText().color());
    const QString text = m_suggestions.isEmpty() ? QStringLiteral("Autocomplete popup") : m_suggestions.join(QStringLiteral(""));
    painter.drawText(rect().adjusted(8,8,-8,-8), Qt::AlignLeft | Qt::AlignTop, text);
}
} // namespace QtMaterial
