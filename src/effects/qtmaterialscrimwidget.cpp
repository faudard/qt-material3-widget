#include "qtmaterial/effects/qtmaterialscrimwidget.h"

#include <QPainter>

#include "qtmaterial/theme/qtmaterialcolortoken.h"

namespace QtMaterial {

QtMaterialScrimWidget::QtMaterialScrimWidget(QWidget* parent)
    : QtMaterialWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    applyThemeDefault();
}

QtMaterialScrimWidget::~QtMaterialScrimWidget() = default;

QColor QtMaterialScrimWidget::scrimColor() const
{
    return m_scrimColor;
}

void QtMaterialScrimWidget::setScrimColor(const QColor& color)
{
    if (m_scrimColor == color && m_hasExplicitScrimColor) {
        return;
    }

    m_hasExplicitScrimColor = true;
    m_scrimColor = color;
    update();
}

void QtMaterialScrimWidget::clearExplicitScrimColor()
{
    m_hasExplicitScrimColor = false;
    applyThemeDefault();
}

void QtMaterialScrimWidget::themeChangedEvent(const QtMaterial::Theme& theme)
{
    QtMaterialWidget::themeChangedEvent(theme);

    if (!m_hasExplicitScrimColor) {
        applyThemeDefault();
    }
}

void QtMaterialScrimWidget::applyThemeDefault()
{
    m_scrimColor = theme().colorScheme().color(ColorRole::Scrim);
    update();
}

void QtMaterialScrimWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.fillRect(rect(), m_scrimColor);
}

} // namespace QtMaterial