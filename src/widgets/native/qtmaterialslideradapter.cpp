#include "qtmaterial/widgets/native/qtmaterialslideradapter.h"

#include <QApplication>
#include <QDynamicPropertyChangeEvent>
#include <QPainter>
#include <QPointer>
#include <QProxyStyle>
#include <QSlider>
#include <QStyleOptionSlider>
#include <QVariant>
#include <QWidget>

#include "../resolution/qtmaterialsliderspecresolution_p.h"
#include "qtmaterial/core/private/qtmaterialthemecontextbinding_p.h"

namespace QtMaterial {
namespace {

constexpr char kAppliedProperty[] = "qtm3MaterialSlider";
constexpr char kDensityProperty[] = "qtm3MaterialDensity";
constexpr char kOptOutProperty[] = "qtm3MaterialOptOut";
constexpr char kStateObjectName[] = "_qtm3_native_slider_adapter_state";

QString densityName(Density density)
{
    switch (density) {
    case Density::Compact:
        return QStringLiteral("compact");
    case Density::Comfortable:
        return QStringLiteral("comfortable");
    case Density::Default:
    default:
        return QStringLiteral("default");
    }
}

Density densityFromProperty(const QVariant& value)
{
    const QString name = value.toString().trimmed().toLower();
    if (name == QStringLiteral("compact")) {
        return Density::Compact;
    }
    if (name == QStringLiteral("comfortable")) {
        return Density::Comfortable;
    }
    return Density::Default;
}

qreal stateLayerOpacity(
    const QStyleOptionSlider& option,
    const SliderSpec& spec)
{
    if (!(option.state & QStyle::State_Enabled)) {
        return 0.0;
    }

    const bool handleActive =
        option.activeSubControls & QStyle::SC_SliderHandle;

    if ((option.state & QStyle::State_Sunken)
        && handleActive) {
        return spec.pressStateLayerOpacity;
    }

    qreal opacity = 0.0;
    if ((option.state & QStyle::State_MouseOver)
        && handleActive) {
        opacity = qMax(
            opacity,
            spec.hoverStateLayerOpacity);
    }
    if (option.state & QStyle::State_HasFocus) {
        opacity = qMax(
            opacity,
            spec.focusStateLayerOpacity);
    }
    return opacity;
}

QPointF centerForRect(const QRect& rect)
{
    return QPointF(
        rect.center().x(),
        rect.center().y());
}

class NativeSliderProxyStyle final : public QProxyStyle
{
public:
    explicit NativeSliderProxyStyle(const SliderSpec& spec)
        : QProxyStyle()
        , m_spec(spec)
    {
    }

    void setResolvedSpec(const SliderSpec& spec)
    {
        m_spec = spec;
    }

    QRect subControlRect(
        ComplexControl control,
        const QStyleOptionComplex* option,
        SubControl subControl,
        const QWidget* widget = nullptr) const override
    {
        const auto* slider =
            qobject_cast<const QSlider*>(widget);
        const auto* sliderOption =
            qstyleoption_cast<const QStyleOptionSlider*>(
                option);

        if (control != CC_Slider
            || !slider
            || !sliderOption
            || !QtMaterialSliderAdapter::isApplied(slider)) {
            return QProxyStyle::subControlRect(
                control,
                option,
                subControl,
                widget);
        }

        const QRect bounds = sliderOption->rect;
        const int diameter = m_spec.handleDiameter;
        const int radius = diameter / 2;

        if (subControl == SC_SliderGroove) {
            if (sliderOption->orientation == Qt::Horizontal) {
                const int left = bounds.left() + radius;
                const int right = bounds.right() - radius;
                return QRect(
                    left,
                    bounds.center().y()
                        - m_spec.trackThickness / 2,
                    qMax(1, right - left + 1),
                    qMax(1, m_spec.trackThickness));
            }

            const int top = bounds.top() + radius;
            const int bottom = bounds.bottom() - radius;
            return QRect(
                bounds.center().x()
                    - m_spec.trackThickness / 2,
                top,
                qMax(1, m_spec.trackThickness),
                qMax(1, bottom - top + 1));
        }

        if (subControl == SC_SliderHandle) {
            const QRect groove = subControlRect(
                CC_Slider,
                sliderOption,
                SC_SliderGroove,
                widget);

            if (sliderOption->orientation == Qt::Horizontal) {
                const int span =
                    qMax(0, groove.width() - 1);
                const int position =
                    QStyle::sliderPositionFromValue(
                        sliderOption->minimum,
                        sliderOption->maximum,
                        sliderOption->sliderPosition,
                        span,
                        sliderOption->upsideDown);
                const int centerX =
                    groove.left() + position;
                return QRect(
                    centerX - radius,
                    bounds.center().y() - radius,
                    diameter,
                    diameter);
            }

            const int span =
                qMax(0, groove.height() - 1);
            const int position =
                QStyle::sliderPositionFromValue(
                    sliderOption->minimum,
                    sliderOption->maximum,
                    sliderOption->sliderPosition,
                    span,
                    sliderOption->upsideDown);
            const int centerY =
                groove.top() + position;
            return QRect(
                bounds.center().x() - radius,
                centerY - radius,
                diameter,
                diameter);
        }

        return QProxyStyle::subControlRect(
            control,
            option,
            subControl,
            widget);
    }

    SubControl hitTestComplexControl(
        ComplexControl control,
        const QStyleOptionComplex* option,
        const QPoint& position,
        const QWidget* widget = nullptr) const override
    {
        const auto* slider =
            qobject_cast<const QSlider*>(widget);
        const auto* sliderOption =
            qstyleoption_cast<const QStyleOptionSlider*>(
                option);

        if (control == CC_Slider
            && slider
            && sliderOption
            && QtMaterialSliderAdapter::isApplied(slider)) {
            const QRect handle = subControlRect(
                control,
                option,
                SC_SliderHandle,
                widget);
            if (handle.adjusted(-4, -4, 4, 4)
                    .contains(position)) {
                return SC_SliderHandle;
            }

            const QRect groove = subControlRect(
                control,
                option,
                SC_SliderGroove,
                widget);
            const int padding =
                qMax(4, m_spec.touchTarget.height() / 4);
            if (groove.adjusted(
                    -padding,
                    -padding,
                    padding,
                    padding)
                    .contains(position)) {
                return SC_SliderGroove;
            }
        }

        return QProxyStyle::hitTestComplexControl(
            control,
            option,
            position,
            widget);
    }

    int pixelMetric(
        PixelMetric metric,
        const QStyleOption* option = nullptr,
        const QWidget* widget = nullptr) const override
    {
        const auto* slider =
            qobject_cast<const QSlider*>(widget);
        const auto* sliderOption =
            qstyleoption_cast<const QStyleOptionSlider*>(
                option);

        if (!slider
            || !QtMaterialSliderAdapter::isApplied(slider)) {
            return QProxyStyle::pixelMetric(
                metric,
                option,
                widget);
        }

        if (metric == PM_SliderLength) {
            return m_spec.handleDiameter;
        }
        if (metric == PM_SliderThickness) {
            return sliderOption
                       && sliderOption->orientation
                              == Qt::Vertical
                ? m_spec.touchTarget.width()
                : m_spec.touchTarget.height();
        }
        if (metric == PM_SliderSpaceAvailable
            && sliderOption) {
            const int extent =
                sliderOption->orientation
                        == Qt::Horizontal
                    ? sliderOption->rect.width()
                    : sliderOption->rect.height();
            return qMax(
                0,
                extent - m_spec.handleDiameter);
        }

        return QProxyStyle::pixelMetric(
            metric,
            option,
            widget);
    }

    QSize sizeFromContents(
        ContentsType type,
        const QStyleOption* option,
        const QSize& contentsSize,
        const QWidget* widget = nullptr) const override
    {
        QSize size = QProxyStyle::sizeFromContents(
            type,
            option,
            contentsSize,
            widget);

        const auto* slider =
            qobject_cast<const QSlider*>(widget);
        const auto* sliderOption =
            qstyleoption_cast<const QStyleOptionSlider*>(
                option);

        if (type != CT_Slider
            || !slider
            || !sliderOption
            || !QtMaterialSliderAdapter::isApplied(slider)) {
            return size;
        }

        if (sliderOption->orientation == Qt::Horizontal) {
            size.setHeight(qMax(
                size.height(),
                m_spec.touchTarget.height()));
        } else {
            size.setWidth(qMax(
                size.width(),
                m_spec.touchTarget.width()));
        }
        return size;
    }

    void drawComplexControl(
        ComplexControl control,
        const QStyleOptionComplex* option,
        QPainter* painter,
        const QWidget* widget = nullptr) const override
    {
        const auto* slider =
            qobject_cast<const QSlider*>(widget);
        const auto* sliderOption =
            qstyleoption_cast<const QStyleOptionSlider*>(
                option);

        if (control != CC_Slider
            || !slider
            || !sliderOption
            || !QtMaterialSliderAdapter::isApplied(slider)) {
            QProxyStyle::drawComplexControl(
                control,
                option,
                painter,
                widget);
            return;
        }

        const bool enabled =
            sliderOption->state & QStyle::State_Enabled;
        const QColor inactiveTrack =
            enabled
                ? m_spec.inactiveTrackColor
                : m_spec.disabledInactiveTrackColor;
        const QColor activeTrack =
            enabled
                ? m_spec.activeTrackColor
                : m_spec.disabledActiveTrackColor;
        const QColor handleColor =
            enabled
                ? m_spec.handleColor
                : m_spec.disabledHandleColor;

        const QRect groove = subControlRect(
            CC_Slider,
            sliderOption,
            SC_SliderGroove,
            widget);
        const QRect handle = subControlRect(
            CC_Slider,
            sliderOption,
            SC_SliderHandle,
            widget);

        QStyleOptionSlider minimumOption(*sliderOption);
        minimumOption.sliderPosition =
            sliderOption->minimum;
        minimumOption.sliderValue =
            sliderOption->minimum;
        const QRect minimumHandle = subControlRect(
            CC_Slider,
            &minimumOption,
            SC_SliderHandle,
            widget);

        painter->save();
        painter->setRenderHint(
            QPainter::Antialiasing,
            true);

        QPen inactivePen(
            inactiveTrack,
            m_spec.trackThickness,
            Qt::SolidLine,
            Qt::RoundCap);
        painter->setPen(inactivePen);

        if (sliderOption->orientation
            == Qt::Horizontal) {
            const qreal y = groove.center().y();
            painter->drawLine(
                QPointF(groove.left(), y),
                QPointF(groove.right(), y));
        } else {
            const qreal x = groove.center().x();
            painter->drawLine(
                QPointF(x, groove.top()),
                QPointF(x, groove.bottom()));
        }

        QPen activePen(
            activeTrack,
            m_spec.trackThickness,
            Qt::SolidLine,
            Qt::RoundCap);
        painter->setPen(activePen);
        painter->drawLine(
            centerForRect(minimumHandle),
            centerForRect(handle));

        const qreal layerOpacity =
            stateLayerOpacity(
                *sliderOption,
                m_spec);
        if (layerOpacity > 0.0) {
            QColor layer = m_spec.stateLayerColor;
            layer.setAlphaF(
                qBound<qreal>(
                    0.0,
                    layer.alphaF() * layerOpacity,
                    1.0));
            painter->setPen(Qt::NoPen);
            painter->setBrush(layer);
            const qreal radius =
                m_spec.stateLayerSize / 2.0;
            painter->drawEllipse(
                centerForRect(handle),
                radius,
                radius);
        }

        painter->setPen(Qt::NoPen);
        painter->setBrush(handleColor);
        painter->drawEllipse(QRectF(handle));

        if (enabled
            && (sliderOption->state
                & QStyle::State_HasFocus)) {
            painter->setBrush(Qt::NoBrush);
            painter->setPen(QPen(
                m_spec.focusRingColor,
                m_spec.focusRingWidth));
            const qreal radius =
                m_spec.handleDiameter / 2.0 + 4.0;
            painter->drawEllipse(
                centerForRect(handle),
                radius,
                radius);
        }

        painter->restore();

        if ((sliderOption->subControls
             & SC_SliderTickmarks)
            && slider->tickPosition()
                   != QSlider::NoTicks) {
            QStyleOptionSlider ticks(*sliderOption);
            ticks.subControls = SC_SliderTickmarks;
            QProxyStyle::drawComplexControl(
                CC_Slider,
                &ticks,
                painter,
                widget);
        }
    }

private:
    SliderSpec m_spec;
};

class SliderAdapterState final : public QObject
{
public:
    explicit SliderAdapterState(QSlider* slider)
        : QObject(slider)
        , m_slider(slider)
        , m_previousStyle(
              slider ? slider->style() : nullptr)
        , m_themeBinding(
              new QtMaterialThemeContextBinding(
                  slider,
                  this))
        , m_style(new NativeSliderProxyStyle(
              resolvedSpec()))
    {
        setObjectName(
            QString::fromLatin1(kStateObjectName));
        m_style->setParent(this);

        if (m_slider) {
            m_slider->installEventFilter(this);
            m_slider->setStyle(m_style);
        }

        QObject::connect(
            m_themeBinding,
            &QtMaterialThemeContextBinding::themeChanged,
            this,
            [this]() { refreshResolvedSpec(); });
        QObject::connect(
            m_themeBinding,
            &QtMaterialThemeContextBinding::
                effectiveThemeContextChanged,
            this,
            [this]() { refreshResolvedSpec(); });
    }

    void restore()
    {
        if (!m_slider) {
            return;
        }

        m_slider->removeEventFilter(this);
        if (m_slider->style() == m_style) {
            QStyle* restoreStyle =
                m_previousStyle.data();
            if (!restoreStyle) {
                restoreStyle =
                    QApplication::style();
            }
            m_slider->setStyle(restoreStyle);
        }
    }

protected:
    bool eventFilter(
        QObject* watched,
        QEvent* event) override
    {
        if (watched == m_slider
            && event->type()
                   == QEvent::DynamicPropertyChange) {
            const auto* dynamic =
                static_cast<
                    QDynamicPropertyChangeEvent*>(
                    event);
            const QByteArray name =
                dynamic->propertyName();
            if (name == kDensityProperty
                || name == kAppliedProperty) {
                refreshResolvedSpec();
            }
        }
        return QObject::eventFilter(
            watched,
            event);
    }

private:
    SliderSpec resolvedSpec() const
    {
        Q_ASSERT(m_slider);
        Q_ASSERT(m_themeBinding);
        return SliderSpecResolution::sliderSpec(
            m_themeBinding,
            QtMaterialSliderAdapter::density(
                m_slider));
    }

    void refreshResolvedSpec()
    {
        if (!m_slider
            || !m_style
            || !m_themeBinding) {
            return;
        }

        m_style->setResolvedSpec(
            resolvedSpec());
        m_slider->updateGeometry();
        m_slider->update();
    }

    QPointer<QSlider> m_slider;
    QPointer<QStyle> m_previousStyle;
    QtMaterialThemeContextBinding*
        m_themeBinding = nullptr;
    NativeSliderProxyStyle* m_style = nullptr;
};

SliderAdapterState* adapterState(QSlider* slider)
{
    if (!slider) {
        return nullptr;
    }

    QObject* object =
        slider->findChild<QObject*>(
            QString::fromLatin1(
                kStateObjectName),
            Qt::FindDirectChildrenOnly);
    return static_cast<SliderAdapterState*>(
        object);
}

} // namespace

void QtMaterialSliderAdapter::apply(
    QSlider* slider,
    Density densityValue)
{
    if (!slider) {
        return;
    }

    slider->setProperty(
        kAppliedProperty,
        true);
    slider->setProperty(
        kDensityProperty,
        densityName(densityValue));

    if (!adapterState(slider)) {
        new SliderAdapterState(slider);
    }

    slider->updateGeometry();
    slider->update();
}

void QtMaterialSliderAdapter::remove(
    QSlider* slider)
{
    if (!slider) {
        return;
    }

    if (SliderAdapterState* state =
            adapterState(slider)) {
        state->restore();
        delete state;
    }

    slider->setProperty(
        kAppliedProperty,
        QVariant());
    slider->setProperty(
        kDensityProperty,
        QVariant());
    slider->updateGeometry();
    slider->update();
}

bool QtMaterialSliderAdapter::isApplied(
    const QSlider* slider)
{
    return slider
        && slider->property(
                     kAppliedProperty)
               .toBool();
}

void QtMaterialSliderAdapter::setDensity(
    QSlider* slider,
    Density densityValue)
{
    if (!slider) {
        return;
    }

    if (!isApplied(slider)) {
        apply(slider, densityValue);
        return;
    }

    slider->setProperty(
        kDensityProperty,
        densityName(densityValue));
}

Density QtMaterialSliderAdapter::density(
    const QSlider* slider)
{
    if (!slider) {
        return Density::Default;
    }
    return densityFromProperty(
        slider->property(kDensityProperty));
}

void QtMaterialSliderAdapter::setOptOut(
    QSlider* slider,
    bool excluded)
{
    if (!slider) {
        return;
    }

    if (excluded) {
        remove(slider);
    }
    slider->setProperty(
        kOptOutProperty,
        excluded);
}

bool QtMaterialSliderAdapter::isOptedOut(
    const QSlider* slider)
{
    return slider
        && slider->property(
                     kOptOutProperty)
               .toBool();
}

int QtMaterialSliderAdapter::applyToDescendants(
    QWidget* root,
    Density densityValue)
{
    if (!root) {
        return 0;
    }

    int count = 0;
    if (auto* rootSlider =
            qobject_cast<QSlider*>(root)) {
        if (!isOptedOut(rootSlider)) {
            apply(rootSlider, densityValue);
            ++count;
        }
    }

    const auto sliders =
        root->findChildren<QSlider*>();
    for (QSlider* slider : sliders) {
        if (!slider
            || isOptedOut(slider)) {
            continue;
        }
        apply(slider, densityValue);
        ++count;
    }

    return count;
}

const char*
QtMaterialSliderAdapter::
    appliedPropertyName() noexcept
{
    return kAppliedProperty;
}

const char*
QtMaterialSliderAdapter::
    densityPropertyName() noexcept
{
    return kDensityProperty;
}

const char*
QtMaterialSliderAdapter::
    optOutPropertyName() noexcept
{
    return kOptOutProperty;
}

} // namespace QtMaterial
