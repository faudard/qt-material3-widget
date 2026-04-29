#include "qtmaterial/core/qtmaterialwidget.h"

#include "qtmaterial/core/qtmaterialautomation.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"

namespace QtMaterial {

QtMaterialWidget::QtMaterialWidget(QWidget* parent)
    : QWidget(parent)
{
    setMaterialComponent(QStringLiteral("Widget"));

    QObject::connect(&ThemeManager::instance(),
                     &ThemeManager::themeChanged,
                     this,
                     &QtMaterialWidget::handleThemeChanged);
}

QtMaterialWidget::~QtMaterialWidget() = default;

QString QtMaterialWidget::materialComponent() const { return m_materialComponent; }
QString QtMaterialWidget::materialVariant() const { return m_materialVariant; }
QString QtMaterialWidget::materialRole() const { return m_materialRole; }
QString QtMaterialWidget::materialTestId() const { return m_materialTestId; }
QString QtMaterialWidget::materialState() const { return m_materialState; }


void QtMaterialWidget::setMaterialComponent(const QString& value)
{
    if (m_materialComponent == value)
        return;

    m_materialComponent = value;
    QtMaterialAutomation::setComponent(this, value);
    emit materialMetadataChanged();
}

void QtMaterialWidget::setMaterialVariant(const QString& value)
{
    if (m_materialVariant == value)
        return;

    m_materialVariant = value;
    QtMaterialAutomation::setVariant(this, value);
    emit materialMetadataChanged();
}

void QtMaterialWidget::setMaterialRole(const QString& value)
{
    if (m_materialRole == value)
        return;

    m_materialRole = value;
    QtMaterialAutomation::setRole(this, value);
    emit materialMetadataChanged();
}

void QtMaterialWidget::setMaterialTestId(const QString& value)
{
    if (m_materialTestId == value)
        return;

    m_materialTestId = value;
    QtMaterialAutomation::setTestId(this, value);
    emit materialMetadataChanged();
}

void QtMaterialWidget::setMaterialState(const QString& value)
{
    if (m_materialState == value)
        return;

    m_materialState = value;
    QtMaterialAutomation::setState(this, value);
    emit materialMetadataChanged();
}

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
