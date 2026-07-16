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

void QtMaterialElevatedButton::paintEvent(QPaintEvent* event)
{
    ensurePolished();
    ensureSpecResolved();

    qreal progress = 0.0;
    if (isEnabled()) {
        const QtMaterialInteractionState state = interactionState();
        if (state.isHovered() || state.isFocused()) {
            progress = 1.0;
        } else if (state.isPressed()) {
            progress = 0.65;
        } else {
            progress = 0.45;
        }
    }

    if (progress > 0.0) {
        const ButtonSpec& spec = currentButtonSpec();
        const QRectF visualRect = ButtonRenderHelper::containerRect(rect(), spec).adjusted(1, 1, -1, -1);
        const qreal radius = ButtonRenderHelper::cornerRadius(theme(), spec, visualRect);

        QColor shadowColor = theme().colorScheme().color(ColorRole::Shadow);
        shadowColor.setAlpha(qBound(0, qRound(28.0 + progress * 40.0), 96));

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        QtMaterialShadowRenderer::paintRoundedShadow(
            &painter,
            visualRect,
            radius,
            shadowColor,
            scaledShadowBlur(*this, progress),
            scaledShadowOffset(*this, progress));
    }

    QtMaterialFilledButton::paintEvent(event);
}

} // namespace QtMaterial
