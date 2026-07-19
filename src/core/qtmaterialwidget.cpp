#include "qtmaterial/core/qtmaterialwidget.h"

#include "private/qtmaterialmetadata_p.h"
#include "qtmaterial/foundation/qtmaterialmetadataproperties.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"

namespace QtMaterial {

QtMaterialWidget::QtMaterialWidget(QWidget* parent)
    : QWidget(parent)
{
    setMaterialComponent(QStringLiteral("Widget"));

    refreshThemeContextConnection();
}

QtMaterialWidget::~QtMaterialWidget() = default;

void QtMaterialWidget::setThemeContext(ThemeContext* context)
{
    if (m_themeContext.data() == context) {
        return;
    }

    m_themeContext = context;
    if (!refreshThemeContextConnection()) {
        return;
    }

    emit effectiveThemeContextChanged(effectiveThemeContext());
    themeChangedEvent(theme());
    notifyDescendantThemeContextChange();
}

ThemeContext* QtMaterialWidget::themeContext() const noexcept
{
    return m_themeContext.data();
}

ThemeContext* QtMaterialWidget::effectiveThemeContext() const noexcept
{
    if (m_themeContext) {
        return m_themeContext.data();
    }

    QWidget* ancestor = parentWidget();
    while (ancestor) {
        if (auto* materialParent = qobject_cast<QtMaterialWidget*>(ancestor)) {
            return materialParent->effectiveThemeContext();
        }
        ancestor = ancestor->parentWidget();
    }

    return ThemeManager::instance().defaultContext();
}

bool QtMaterialWidget::event(QEvent* event)
{
    const bool handled = QWidget::event(event);

    if (event && event->type() == QEvent::ParentChange
        && refreshThemeContextConnection()) {
        emit effectiveThemeContextChanged(effectiveThemeContext());
        themeChangedEvent(theme());
        notifyDescendantThemeContextChange();
    }

    return handled;
}

bool QtMaterialWidget::refreshThemeContextConnection()
{
    ThemeContext* nextContext = effectiveThemeContext();
    if (m_effectiveThemeContext.data() == nextContext) {
        return false;
    }

    QObject::disconnect(m_themeChangedConnection);
    QObject::disconnect(m_themeDestroyedConnection);

    m_effectiveThemeContext = nextContext;

    if (nextContext) {
        m_themeChangedConnection = QObject::connect(
            nextContext,
            &ThemeContext::themeChanged,
            this,
            &QtMaterialWidget::handleThemeChanged);

        m_themeDestroyedConnection = QObject::connect(
            nextContext,
            &QObject::destroyed,
            this,
            &QtMaterialWidget::handleThemeContextDestroyed);
    }

    return true;
}

void QtMaterialWidget::notifyDescendantThemeContextChange()
{
    const auto descendants = findChildren<QtMaterialWidget*>();
    for (QtMaterialWidget* descendant : descendants) {
        if (!descendant || !descendant->refreshThemeContextConnection()) {
            continue;
        }

        emit descendant->effectiveThemeContextChanged(
            descendant->effectiveThemeContext());
        descendant->themeChangedEvent(descendant->theme());
    }
}

void QtMaterialWidget::handleThemeContextDestroyed()
{
    m_themeContext.clear();
    m_effectiveThemeContext.clear();

    if (!refreshThemeContextConnection()) {
        return;
    }

    emit effectiveThemeContextChanged(effectiveThemeContext());
    themeChangedEvent(theme());
    notifyDescendantThemeContextChange();
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
    ThemeContext* context = effectiveThemeContext();
    Q_ASSERT(context);
    return context->theme();
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
