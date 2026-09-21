#include "qtmaterial/widgets/inputs/qtmaterialcombobox.h"

namespace QtMaterial {

QtMaterialComboBox::QtMaterialComboBox(QWidget* parent)
    : QComboBox(parent)
{
    setObjectName(QStringLiteral("qtmaterial_combo_box"));
    setFocusPolicy(Qt::StrongFocus);
    setSizeAdjustPolicy(QComboBox::AdjustToContentsOnFirstShow);
    setMinimumHeight(48);
}

QtMaterialComboBox::~QtMaterialComboBox() = default;

QString QtMaterialComboBox::labelText() const { return m_labelText; }

void QtMaterialComboBox::setLabelText(const QString& text)
{
    if (m_labelText == text) {
        return;
    }
    m_labelText = text;
    if (!text.isEmpty()) {
        setAccessibleName(text);
    }
    emit labelTextChanged(text);
}

} // namespace QtMaterial
