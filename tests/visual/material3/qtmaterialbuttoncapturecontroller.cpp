#include "qtmaterialbuttoncapturecontroller_p.h"

#include <QPainter>
#include <QRegion>
#include <QColorSpace>
#include <QtGlobal>

#include <memory>

#include "qtmaterialbuttonmotionhelper_p.h"
#include "qtmaterialcaptureeffectsaccess_p.h"
#include "qtmaterial/effects/qtmaterialripplecontroller.h"
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/foundation/qtmaterialdensity.h"
#include "qtmaterial/specs/qtmaterialbuttonspec.h"
#include "qtmaterial/widgets/buttons/qtmaterialelevatedbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledtonalbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"

namespace QtMaterial::VisualCapturePrivate {
namespace {

const QString kRippleObjectName =
    QStringLiteral("_qtm3_button_ripple_controller");
const QString kStateLayerObjectName =
    QStringLiteral("_qtm3_button_state_layer_transition");
const QString kElevationObjectName =
    QStringLiteral("_qtm3_button_elevation_transition");

class CapturableButtonInterface
{
public:
    virtual ~CapturableButtonInterface() = default;
    virtual QtMaterialTextButton* button() noexcept = 0;
    virtual void applyState(const ButtonCaptureState& state) = 0;
    virtual ButtonSpec resolvedSpec() = 0;
    virtual qreal targetStateLayerOpacity(const ButtonSpec& spec) const = 0;
};

template<typename ButtonType>
class CapturableButton final
    : public ButtonType
    , public CapturableButtonInterface
{
public:
    QtMaterialTextButton* button() noexcept override
    {
        return this;
    }

    void applyState(const ButtonCaptureState& state) override
    {
        this->setDown(false);
        this->setEnabled(state.enabled);
        this->setDown(state.enabled && state.pressed);

        this->interactionState().setEnabled(state.enabled);
        this->interactionState().setHovered(state.hovered);
        this->interactionState().setFocused(state.focused);
        this->interactionState().setPressed(state.pressed);
        this->stateChangedEvent();
    }

    ButtonSpec resolvedSpec() override
    {
        this->ensureSpecResolved();
        return this->currentButtonSpec();
    }

    qreal targetStateLayerOpacity(const ButtonSpec& spec) const override
    {
        return ButtonMotionHelper::targetStateLayerOpacity(
            spec,
            this->interactionState());
    }
};

std::unique_ptr<CapturableButtonInterface> createButton(
    const QString& componentId)
{
    if (componentId == QLatin1String("button.text")) {
        return std::make_unique<CapturableButton<QtMaterialTextButton>>();
    }
    if (componentId == QLatin1String("button.filled")) {
        return std::make_unique<CapturableButton<QtMaterialFilledButton>>();
    }
    if (componentId == QLatin1String("button.filled-tonal")) {
        return std::make_unique<CapturableButton<QtMaterialFilledTonalButton>>();
    }
    if (componentId == QLatin1String("button.outlined")) {
        return std::make_unique<CapturableButton<QtMaterialOutlinedButton>>();
    }
    if (componentId == QLatin1String("button.elevated")) {
        return std::make_unique<CapturableButton<QtMaterialElevatedButton>>();
    }
    return {};
}

bool requiresElevationTransition(const QString& componentId)
{
    return componentId == QLatin1String("button.filled")
        || componentId == QLatin1String("button.filled-tonal")
        || componentId == QLatin1String("button.elevated");
}

qreal elevationProgressFor(
    const ButtonSpec& spec,
    const ButtonCaptureState& state)
{
    if (!state.enabled) {
        return spec.disabledElevationProgress;
    }
    if (state.pressed) {
        return spec.pressElevationProgress;
    }
    if (state.focused) {
        return spec.focusElevationProgress;
    }
    if (state.hovered) {
        return spec.hoverElevationProgress;
    }
    return spec.restingElevationProgress;
}

QPointF feedbackCenter(
    const QWidget& button,
    const QPointF& normalizedOrigin)
{
    const QRectF bounds(button.rect());
    return QPointF(
        bounds.left() + bounds.width() * normalizedOrigin.x(),
        bounds.top() + bounds.height() * normalizedOrigin.y());
}

QImage renderButton(
    QWidget* button,
    const QSize& canvasSize,
    const QRect& widgetRect,
    const QColor& background)
{
    QImage image(canvasSize, QImage::Format_RGBA8888);
    image.setDevicePixelRatio(1.0);
    image.setColorSpace(QColorSpace(QColorSpace::SRgb));
    image.fill(background);

    QPainter painter(&image);
    painter.translate(widgetRect.topLeft());
    button->render(
        &painter,
        QPoint(),
        QRegion(),
        QWidget::DrawChildren);
    painter.end();
    return image;
}

bool fail(QString* errorMessage, const QString& message)
{
    if (errorMessage) {
        *errorMessage = message;
    }
    return false;
}

} // namespace

bool ButtonCaptureController::capture(
    const QString& componentId,
    const ButtonCaptureState& state,
    const Theme& theme,
    const QString& label,
    const QSize& canvasSize,
    const QRect& widgetRect,
    const QColor& background,
    ButtonCaptureResult* result,
    QString* errorMessage) const
{
    if (!result) {
        return fail(errorMessage, QStringLiteral("capture result is null"));
    }
    if (!canvasSize.isValid() || !widgetRect.isValid()
        || !QRect(QPoint(), canvasSize).contains(widgetRect)) {
        return fail(errorMessage, QStringLiteral("capture geometry is invalid"));
    }
    if (!background.isValid()) {
        return fail(errorMessage, QStringLiteral("capture background is invalid"));
    }
    if (state.stateLayerProgress < 0.0 || state.stateLayerProgress > 1.0
        || state.feedbackProgress < 0.0 || state.feedbackProgress > 1.0
        || state.feedbackOrigin.x() < 0.0 || state.feedbackOrigin.x() > 1.0
        || state.feedbackOrigin.y() < 0.0 || state.feedbackOrigin.y() > 1.0) {
        return fail(errorMessage, QStringLiteral("capture phase is outside [0, 1]"));
    }
    if (state.feedbackKind != QLatin1String("none")
        && state.feedbackKind != QLatin1String("state-layer")
        && state.feedbackKind != QLatin1String("ripple")) {
        return fail(errorMessage, QStringLiteral("capture feedback kind is invalid"));
    }

    ThemeContext context(theme);
    auto controlled = createButton(componentId);
    if (!controlled) {
        return fail(
            errorMessage,
            QStringLiteral("unsupported Button component: %1").arg(componentId));
    }

    QtMaterialTextButton* button = controlled->button();
    button->setAttribute(Qt::WA_DontShowOnScreen, true);
    button->setAttribute(Qt::WA_NoSystemBackground, true);
    button->setLayoutDirection(Qt::LeftToRight);
    button->setDensity(Density::Default);
    button->setThemeContext(&context);
    button->setText(label);
    button->setFixedSize(widgetRect.size());
    button->ensurePolished();

    controlled->applyState(state);
    const ButtonSpec spec = controlled->resolvedSpec();
    if (!spec.hasResolvedLabelFont) {
        return fail(errorMessage, QStringLiteral("Button label font is unresolved"));
    }

    auto* stateLayer = button->findChild<QtMaterialTransitionController*>(
        kStateLayerObjectName,
        Qt::FindDirectChildrenOnly);
    auto* ripple = button->findChild<QtMaterialRippleController*>(
        kRippleObjectName,
        Qt::FindDirectChildrenOnly);
    if (!stateLayer || !ripple) {
        return fail(
            errorMessage,
            QStringLiteral("Button capture controllers are not uniquely bound"));
    }

    const qreal stateLayerOpacity =
        controlled->targetStateLayerOpacity(spec)
        * state.stateLayerProgress;
    QtMaterialTransitionControllerCaptureAccess::freeze(
        *stateLayer,
        stateLayerOpacity);

    if (requiresElevationTransition(componentId)) {
        auto* elevation = button->findChild<QtMaterialTransitionController*>(
            kElevationObjectName,
            Qt::FindDirectChildrenOnly);
        if (!elevation) {
            return fail(
                errorMessage,
                QStringLiteral("Button elevation controller is not bound"));
        }
        QtMaterialTransitionControllerCaptureAccess::freeze(
            *elevation,
            elevationProgressFor(spec, state));
    }

    ripple->clear();
    const QImage renderWarmup = renderButton(
        button,
        canvasSize,
        widgetRect,
        background);
    if (renderWarmup.isNull()) {
        return fail(errorMessage, QStringLiteral("Button warm-up render failed"));
    }

    QtMaterialTransitionControllerCaptureAccess::freeze(
        *stateLayer,
        stateLayerOpacity);
    if (state.feedbackKind == QLatin1String("ripple")) {
        QtMaterialRippleControllerCaptureAccess::freeze(
            *ripple,
            feedbackCenter(*button, state.feedbackOrigin),
            state.feedbackProgress);
    } else {
        ripple->clear();
    }

    result->image = renderButton(
        button,
        canvasSize,
        widgetRect,
        background);
    result->resolvedLabelFont = spec.labelFont;
    if (result->image.isNull()) {
        return fail(errorMessage, QStringLiteral("Button render failed"));
    }
    return true;
}

} // namespace QtMaterial::VisualCapturePrivate
