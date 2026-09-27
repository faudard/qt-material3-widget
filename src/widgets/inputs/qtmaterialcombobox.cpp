#include "qtmaterial/widgets/inputs/qtmaterialcombobox.h"

namespace QtMaterial {

class QtMaterialComboBoxPrivate final
{
public:
    QString labelText;
};

QtMaterialComboBox::QtMaterialComboBox(QWidget* parent)
    : QComboBox(parent)
    , d_ptr(std::make_unique<QtMaterialComboBoxPrivate>())
{
    setObjectName(QStringLiteral("qtmaterial_combo_box"));
    setFocusPolicy(Qt::StrongFocus);
    setSizeAdjustPolicy(QComboBox::AdjustToContentsOnFirstShow);
    setMinimumHeight(48);
}

QtMaterialComboBox::~QtMaterialComboBox() = default;

QString QtMaterialComboBox::labelText() const { return d_ptr->labelText; }

void QtMaterialComboBox::setLabelText(const QString& text)
{
    if (d_ptr->labelText == text) {
        return;
    }
    d_ptr->labelText = text;
    if (!text.isEmpty()) {
        setAccessibleName(text);
    }
    emit labelTextChanged(text);
}

} // namespace QtMaterial
