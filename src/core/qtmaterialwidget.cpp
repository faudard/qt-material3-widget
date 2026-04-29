#include "qtmaterial/core/qtmaterialwidget.h"

#include "qtmaterial/core/qtmaterialautomation.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"

namespace QtMaterial {

QtMaterialWidget::QtMaterialWidget(QWidget* parent)
    : QWidget(parent)
{
    QtMaterialAutomation::setComponent(this, QStringLiteral("Widget"));
    QObject::connect(&ThemeManager::instance(), &ThemeManager::themeChanged,
                     this, &QtMaterialWidget::handleThemeChanged);
}

QtMaterialWidget::~QtMaterialWidget() = default;

QString QtMaterialWidget::materialComponent() const { return m_materialComponent; }
QString QtMaterialWidget::materialVariant() const { return m_materialVariant; }
QString QtMaterialWidget::materialRole() const { return m_materialRole; }
QString QtMaterialWidget::materialTestId() const { return m_materialTestId; }
QString QtMaterialWidget::materialState() const { return m_materialState; }

void QtMaterialWidget::setMaterialComponent(const QString& value) { setMetadata(&m_materialComponent, value); }
void QtMaterialWidget::setMaterialVariant(const QString& value) { setMetadata(&m_materialVariant, value); }
void QtMaterialWidget::setMaterialRole(const QString& value) { setMetadata(&m_materialRole, value); }
void QtMaterialWidget::setMaterialTestId(const QString& value) { setMetadata(&m_materialTestId, value); }
void QtMaterialWidget::setMaterialState(const QString& value) { setMetadata(&m_materialState, value); }

const Theme& QtMaterialWidget::theme() const
{
    return ThemeManager::instance().theme();
}

void QtMaterialWidget::themeChangedEvent(const Theme&)
{
    update();
}

void QtMaterialWidget::handleThemeChanged(const Theme& theme)
{
    themeChangedEvent(theme);
}

void QtMaterialWidget::setMetadata(QString* storage, const QString& value)
{
    if (!storage || *storage == value) {
        return;
    }
    *storage = value;
    emit materialMetadataChanged();
}

} // namespace QtMaterial
