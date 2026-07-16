#include "qtmaterial/widgets/buttons/qtmaterialelevatedbutton.h"

#include <QPainter>
#include <QtGlobal>

#include "private/qtmaterialbuttonrenderhelper_p.h"
#include "qtmaterial/effects/qtmaterialshadowrenderer.h"
#include "qtmaterial/specs/qtmaterialbuttonspecresolver.h"

namespace QtMaterial {

namespace {

int scaledShadowBlur(const QWidget& widget, qreal progress)
{
    const qreal dpr = qMax<qreal>(widget.devicePixelRatioF(), 1.0);
    return qMax(1, qRound((4.0 + progress * 6.0) * dpr));
}

int scaledShadowOffset(const QWidget& widget, qreal progress)
{
    const qreal dpr = qMax<qreal>(widget.devicePixelRatioF(), 1.0);
    return qMax(1, qRound((1.0 + progress * 2.0) * dpr));
}

} // namespace

QtMaterialElevatedButton::QtMaterialElevatedButton(QWidget* parent)
    : QtMaterialFilledButton(parent)
{
    setMaterialComponent(QStringLiteral("button"));
    setMaterialVariant(QStringLiteral("elevated"));
    setMaterialRole(QStringLiteral("action"));
}

QtMaterialElevatedButton::QtMaterialElevatedButton(const QString& text, QWidget* parent)
    : QtMaterialElevatedButton(parent)
{
    setText(text);
}

QtMaterialElevatedButton::QtMaterialElevatedButton(const QIcon& icon, const QString& text, QWidget* parent)
    : QtMaterialElevatedButton(text, parent)
{
    setIcon(icon);
}

QtMaterialElevatedButton::~QtMaterialElevatedButton() = default;

ButtonSpec QtMaterialElevatedButton::resolveButtonSpec() const
{
    ButtonSpecResolver factory;
    return factory.elevatedButtonSpec(theme(), density());
}


void QtMaterialElevatedButton::paintEvent(
    QPaintEvent* event)
{
    ensureSpecResolved();

    QPainter painter(this);
    painter.setRenderHint(
        QPainter::Antialiasing,
        true);

    const QRectF visualRect =
        ButtonRenderHelper::containerRect(
            rect(),
            m_spec)
            .adjusted(1, 1, -1, -1);
    const qreal radius =
        ButtonRenderHelper::cornerRadius(
            m_spec,
            visualRect);

    QtMaterialShadowRenderer::paintRoundedShadow(
        &painter,
        visualRect,
        radius,
        m_spec.shadowColor,
        6,
        1);

    QtMaterialFilledButton::paintEvent(event);
}

} // namespace QtMaterial
