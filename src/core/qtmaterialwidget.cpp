#include "qtmaterial/core/qtmaterialwidget.h"
#include "qtmaterial/core/qtmaterialthemecontextbinding.h"
#include "private/qtmaterialmetadata_p.h"
#include "qtmaterial/foundation/qtmaterialmetadataproperties.h"
namespace QtMaterial {

QtMaterialWidget::QtMaterialWidget(QWidget* parent)
    : QWidget(parent)
    , m_themeBinding(new QtMaterialThemeContextBinding(this, this))
{
    QObject::connect(
        m_themeBinding,
        &QtMaterialThemeContextBinding::effectiveThemeContextChanged,
        this,
        &QtMaterialWidget::effectiveThemeContextChanged);

    QObject::connect(
        m_themeBinding,
        &QtMaterialThemeContextBinding::themeChanged,
        this,
        &QtMaterialWidget::handleThemeChanged);

    setMaterialComponent(QStringLiteral("Widget"));
}
QtMaterialWidget::~QtMaterialWidget() = default;

void QtMaterialWidget::setThemeContext(ThemeContext* context)
{
    m_themeBinding->setThemeContext(context);
}

ThemeContext* QtMaterialWidget::themeContext() const noexcept
{
    return m_themeBinding->themeContext();
}

ThemeContext* QtMaterialWidget::effectiveThemeContext() const noexcept
{
    return m_themeBinding->effectiveThemeContext();
}

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
    MetadataPrivate::setStringProperty(this, QtMaterialMetadataPropertyNames::component(), value);
    emit materialMetadataChanged();
}

void QtMaterialWidget::setMaterialVariant(const QString& value)
{
    if (m_materialVariant == value)
        return;

    m_materialVariant = value;
    MetadataPrivate::setStringProperty(this, QtMaterialMetadataPropertyNames::variant(), value);
    emit materialMetadataChanged();
}

void QtMaterialWidget::setMaterialRole(const QString& value)
{
    if (m_materialRole == value)
        return;

    m_materialRole = value;
    MetadataPrivate::setStringProperty(this, QtMaterialMetadataPropertyNames::role(), value);
    emit materialMetadataChanged();
}

void QtMaterialWidget::setMaterialTestId(const QString& value)
{
    if (m_materialTestId == value)
        return;

    m_materialTestId = value;
    MetadataPrivate::setStringProperty(this, QtMaterialMetadataPropertyNames::testId(), value);
    emit materialMetadataChanged();
}

void QtMaterialWidget::setMaterialState(const QString& value)
{
    if (m_materialState == value)
        return;

    m_materialState = value;
    MetadataPrivate::setStringProperty(this, QtMaterialMetadataPropertyNames::state(), value);
    emit materialMetadataChanged();
}

const Theme& QtMaterialWidget::theme() const
{
    return m_themeBinding->theme();
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
