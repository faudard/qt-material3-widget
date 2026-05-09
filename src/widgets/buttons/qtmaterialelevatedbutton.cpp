#include "qtmaterial/widgets/buttons/qtmaterialelevatedbutton.h"

#include <QPainter>
#include <QtGlobal>

#include "private/qtmaterialbuttonrenderhelper_p.h"
#include "qtmaterial/effects/qtmaterialshadowrenderer.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"

namespace QtMaterial {

namespace {

qreal elevatedShadowProgress(const QtMaterialElevatedButton& button)
{
    if (!button.isEnabled()) {
        return 0.0;
    }

    const QtMaterialInteractionState state = button.interactionState();
    if (state.isPressed()) {
        return 0.65;
    }

    if (state.isHovered() || state.isFocused()) {
        return 1.0;
    }

    return 0.45;
}

int scaledShadowBlur(const QtMaterialElevatedButton& button, qreal progress)
{
    const qreal dpr = qMax<qreal>(button.devicePixelRatioF(), 1.0);
    return qMax(1, qRound((4.0 + progress * 6.0) * dpr));
}

int scaledShadowOffset(const QtMaterialElevatedButton& button, qreal progress)
{
    const qreal dpr = qMax<qreal>(button.devicePixelRatioF(), 1.0);
    return qMax(1, qRound((1.0 + progress * 2.0) * dpr));
}

QColor elevatedShadowColor(const QtMaterialElevatedButton& button, qreal progress)
{
    QColor color = button.theme().colorScheme().color(ColorRole::Shadow);
    color.setAlpha(qBound(0, qRound(28.0 + progress * 40.0), 96));
    return color;
}

} // namespace

QtMaterialElevatedButton::QtMaterialElevatedButton(QWidget* parent)
    : QtMaterialFilledButton(parent)
{
}

QtMaterialElevatedButton::~QtMaterialElevatedButton() = default;

ButtonSpec QtMaterialElevatedButton::resolveButtonSpec() const
{
    SpecFactory factory;
    return factory.elevatedButtonSpec(theme(), density());
}

void QtMaterialElevatedButton::paintEvent(QPaintEvent* event)
{
    ensurePolished();
    ensureSpecResolved();

    const qreal progress = elevatedShadowProgress(*this);
    if (progress > 0.0) {
        const ButtonSpec& spec = currentButtonSpec();
        const QRectF visualRect = ButtonRenderHelper::containerRect(rect(), spec).adjusted(1, 1, -1, -1);
        const qreal radius = ButtonRenderHelper::cornerRadius(theme(), spec, visualRect);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        QtMaterialShadowRenderer::paintRoundedShadow(
            &painter,
            visualRect,
            radius,
            elevatedShadowColor(*this, progress),
            scaledShadowBlur(*this, progress),
            scaledShadowOffset(*this, progress));
    }

    QtMaterialFilledButton::paintEvent(event);
}

} // namespace QtMaterial
