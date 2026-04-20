#include "qtmaterial/widgets/surfaces/qtmaterialcard.h"

#include <QPainter>

#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "private/qtmaterialsurfacerenderhelper_p.h"

namespace QtMaterial {

QtMaterialCard::QtMaterialCard(QWidget* parent)
    : QtMaterialSurface(parent)
{
    setMinimumSize(160, 100);
}

QtMaterialCard::~QtMaterialCard() = default;

QSize QtMaterialCard::sizeHint() const
{
    return QSize(240, 140);
}

QSize QtMaterialCard::minimumSizeHint() const
{
    return QSize(160, 100);
}

void QtMaterialCard::themeChangedEvent(const Theme& theme)
{
    QtMaterialSurface::themeChangedEvent(theme);
    m_specDirty = true;
}

void QtMaterialCard::invalidateResolvedSpec()
{
    m_specDirty = true;
}

void QtMaterialCard::ensureSpecResolved() const
{
    if (!m_specDirty) {
        return;
    }
    SpecFactory factory;
    m_spec = factory.cardSpec(theme());
    m_specDirty = false;
}

void QtMaterialCard::paintEvent(QPaintEvent*)
{
    ensureSpecResolved();
    QPainter painter(this);
    QtMaterialSurfaceRenderHelper::paintFrame(&painter, QtMaterialSurfaceRenderHelper::cardFrame(theme(), m_spec, rect()));
    painter.setPen(m_spec.contentColor);
    painter.drawText(rect().marginsRemoved(m_spec.contentPadding), Qt::AlignLeft | Qt::AlignTop, QStringLiteral("Card"));
}

} // namespace QtMaterial
