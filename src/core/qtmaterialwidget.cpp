#include "qtmaterial/core/qtmaterialwidget.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"
namespace QtMaterial {
QtMaterialWidget::QtMaterialWidget(QWidget* parent) : QWidget(parent)
{
    QObject::connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, &QtMaterialWidget::handleThemeChanged);
}
QtMaterialWidget::~QtMaterialWidget() = default;
const Theme& QtMaterialWidget::theme() const { return ThemeManager::instance().theme(); }
void QtMaterialWidget::themeChangedEvent(const Theme&) { update(); }
void QtMaterialWidget::handleThemeChanged(const Theme& theme) { themeChangedEvent(theme); }
} // namespace QtMaterial
