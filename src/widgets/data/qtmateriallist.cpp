#include "qtmaterial/widgets/data/qtmateriallist.h"
#include <QVBoxLayout>
namespace QtMaterial {
QtMaterialList::QtMaterialList(QWidget* parent) : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
}
QtMaterialList::~QtMaterialList() = default;
} // namespace QtMaterial
