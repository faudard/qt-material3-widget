#include "qtmaterial/widgets/native/qtmaterialtoolbuttonadapter.h"

#include <QApplication>
#include <QDynamicPropertyChangeEvent>
#include <QFontMetrics>
#include <QPainter>
#include <QPainterPath>
#include <QPointer>
#include <QProxyStyle>
#include <QStyleOption>
#include <QToolButton>
#include <QVariant>
#include <QWidget>

#include "../resolution/qtmaterialbuttonspecresolution_p.h"
#include "qtmaterial/core/private/qtmaterialthemecontextbinding_p.h"

namespace QtMaterial {
namespace {

constexpr char kAppliedProperty[] = "qtm3MaterialToolButton";
constexpr char kVariantProperty[] = "qtm3MaterialVariant";
constexpr char kDensityProperty[] = "qtm3MaterialDensity";
constexpr char kOptOutProperty[] = "qtm3MaterialOptOut";
constexpr char kStateObjectName[] = "_qtm3_native_toolbutton_adapter_state";

QString variantName(ButtonVariant variant)
{
    switch (variant) {
    case ButtonVariant::Filled:
        return QStringLiteral("filled");
    case ButtonVariant::FilledTonal:
        return QStringLiteral("filled-tonal");
    case ButtonVariant::Outlined:
        return QStringLiteral("outlined");
    case ButtonVariant::Elevated:
        return QStringLiteral("elevated");
    case ButtonVariant::Text:
    default:
        return QStringLiteral("text");
    }
}

ButtonVariant variantFromProperty(const QVariant& value)
{
    const QString name = value.toString().trimmed().toLower();
    if (name == QStringLiteral("filled")) {
        return ButtonVariant::Filled;
    }
    if (name == QStringLiteral("filled-tonal")
        || name == QStringLiteral("filledtonal")
        || name == QStringLiteral("tonal")) {
        return ButtonVariant::FilledTonal;
    }
    if (name == QStringLiteral("outlined")) {
        return ButtonVariant::Outlined;
    }
    if (name == QStringLiteral("elevated")) {
        return ButtonVariant::Elevated;
    }
    return ButtonVariant::Text;
}

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

QRectF containerRect(const QRect& rect, const ButtonSpec& spec)
{
    QRectF result(rect);
    const qreal height =
        qMin<qreal>(spec.containerHeight, result.height());
    result.setTop(result.center().y() - height / 2.0);
    result.setHeight(height);
    return result;
}

qreal cornerRadius(const ButtonSpec& spec, const QRectF& rect)
{
    if (spec.cornerRadius < 0.0) {
        return rect.height() / 2.0;
    }
    return qMin<qreal>(
        spec.cornerRadius,
        rect.height() / 2.0);
}

qreal stateOpacity(
    const QStyleOptionToolButton& option,
    const ButtonSpec& spec)
{
    if (!(option.state & QStyle::State_Enabled)) {
        return 0.0;
    }
    if (option.state & QStyle::State_Sunken) {
        return spec.pressStateLayerOpacity;
    }

    qreal opacity = 0.0;
    if (option.state & QStyle::State_MouseOver) {
        opacity =
            qMax(opacity, spec.hoverStateLayerOpacity);
    }
    if (option.state & QStyle::State_HasFocus) {
        opacity =
            qMax(opacity, spec.focusStateLayerOpacity);
    }
    if (option.state & QStyle::State_On) {
        opacity =
            qMax(opacity, spec.focusStateLayerOpacity);
    }
    return opacity;
}

bool hasSplitMenu(const QStyleOptionToolButton& option)
{
    return option.features
        & QStyleOptionToolButton::MenuButtonPopup;
}

bool hasAnyMenu(const QStyleOptionToolButton& option)
{
    return option.features
        & QStyleOptionToolButton::HasMenu;
}

class NativeToolButtonProxyStyle final : public QProxyStyle
{
public:
    explicit NativeToolButtonProxyStyle(const ButtonSpec& spec)
        : QProxyStyle()
        , m_spec(spec)
    {
    }

    void setResolvedSpec(const ButtonSpec& spec)
    {
        m_spec = spec;
    }

    QRect subControlRect(
        ComplexControl control,
        const QStyleOptionComplex* option,
        SubControl subControl,
        const QWidget* widget = nullptr) const override
    {
        const auto* button =
            qobject_cast<const QToolButton*>(widget);
        const auto* toolOption =
            qstyleoption_cast<const QStyleOptionToolButton*>(option);

        if (control != CC_ToolButton
            || !button
            || !toolOption
            || !QtMaterialToolButtonAdapter::isApplied(button)) {
            return QProxyStyle::subControlRect(
                control,
                option,
                subControl,
                widget);
        }

        if (!hasSplitMenu(*toolOption)) {
            if (subControl == SC_ToolButton) {
                return toolOption->rect;
            }
            return QProxyStyle::subControlRect(
                control,
                option,
                subControl,
                widget);
        }

        const int menuWidth = qMax(
            24,
            pixelMetric(
                PM_MenuButtonIndicator,
                toolOption,
                widget) + 10);
        const QRect bounds = toolOption->rect;
        const QRect logicalMenu(
            bounds.right() - menuWidth + 1,
            bounds.top(),
            menuWidth,
            bounds.height());
        const QRect menuRect =
            QStyle::visualRect(
                toolOption->direction,
                bounds,
                logicalMenu);

        if (subControl == SC_ToolButtonMenu) {
            return menuRect;
        }

        if (subControl == SC_ToolButton) {
            QRect mainRect = bounds;
            if (toolOption->direction == Qt::RightToLeft) {
                mainRect.setLeft(menuRect.right() + 1);
            } else {
                mainRect.setRight(menuRect.left() - 1);
            }
            return mainRect;
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
        const auto* button =
            qobject_cast<const QToolButton*>(widget);
        const auto* toolOption =
            qstyleoption_cast<const QStyleOptionToolButton*>(option);

        if (control == CC_ToolButton
            && button
            && toolOption
            && QtMaterialToolButtonAdapter::isApplied(button)
            && hasSplitMenu(*toolOption)) {
            if (subControlRect(
                    control,
                    option,
                    SC_ToolButtonMenu,
                    widget)
                    .contains(position)) {
                return SC_ToolButtonMenu;
            }
            if (subControlRect(
                    control,
                    option,
                    SC_ToolButton,
                    widget)
                    .contains(position)) {
                return SC_ToolButton;
            }
        }

        return QProxyStyle::hitTestComplexControl(
            control,
            option,
            position,
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

        const auto* button =
            qobject_cast<const QToolButton*>(widget);
        const auto* toolOption =
            qstyleoption_cast<const QStyleOptionToolButton*>(option);

        if (type != CT_ToolButton
            || !button
            || !toolOption
            || !QtMaterialToolButtonAdapter::isApplied(button)) {
            return size;
        }

        size.setHeight(
            qMax(
                size.height(),
                m_spec.touchTarget.height()));
        size.setWidth(
            qMax(
                size.width(),
                m_spec.touchTarget.width()));

        const bool showsText =
            toolOption->toolButtonStyle
                != Qt::ToolButtonIconOnly
            && !toolOption->text.isEmpty();
        if (showsText) {
            size.rwidth() +=
                qMax(0, m_spec.horizontalPadding / 2);
        }
        if (hasAnyMenu(*toolOption)) {
            size.rwidth() += qMax(
                0,
                pixelMetric(
                    PM_MenuButtonIndicator,
                    toolOption,
                    widget) / 2);
        }

        return size;
    }

    void drawComplexControl(
        ComplexControl control,
        const QStyleOptionComplex* option,
        QPainter* painter,
        const QWidget* widget = nullptr) const override
    {
        const auto* button =
            qobject_cast<const QToolButton*>(widget);
        const auto* toolOption =
            qstyleoption_cast<const QStyleOptionToolButton*>(option);

        if (control != CC_ToolButton
            || !button
            || !toolOption
            || !QtMaterialToolButtonAdapter::isApplied(button)) {
            QProxyStyle::drawComplexControl(
                control,
                option,
                painter,
                widget);
            return;
        }

        const QRectF visualRect =
            containerRect(toolOption->rect, m_spec);
        const qreal radius =
            cornerRadius(m_spec, visualRect);

        QPainterPath path;
        path.addRoundedRect(
            visualRect,
            radius,
            radius);

        const bool enabled =
            toolOption->state & QStyle::State_Enabled;
        const QColor container =
            enabled
                ? m_spec.containerColor
                : m_spec.disabledContainerColor;

        painter->save();
        painter->setRenderHint(
            QPainter::Antialiasing,
            true);
        painter->setPen(Qt::NoPen);
        painter->setBrush(container);
        painter->drawPath(path);

        const qreal layerOpacity =
            stateOpacity(*toolOption, m_spec);
        if (layerOpacity > 0.0) {
            QColor layer = m_spec.stateLayerColor;
            layer.setAlphaF(
                qBound<qreal>(
                    0.0,
                    layer.alphaF() * layerOpacity,
                    1.0));
            painter->setBrush(layer);
            painter->drawPath(path);
        }

        const QColor outline =
            enabled
                ? m_spec.outlineColor
                : m_spec.disabledOutlineColor;
        if (m_spec.outlineWidth > 0.0
            && outline.alpha() > 0) {
            QPen pen(
                outline,
                m_spec.outlineWidth);
            pen.setJoinStyle(Qt::RoundJoin);
            painter->setPen(pen);
            painter->setBrush(Qt::NoBrush);
            painter->drawPath(path);
        }
        painter->restore();

        QStyleOptionToolButton labelOption(*toolOption);
        QRect labelRect = toolOption->rect;
        if (hasSplitMenu(*toolOption)) {
            labelRect = subControlRect(
                CC_ToolButton,
                toolOption,
                SC_ToolButton,
                widget);
        } else if (hasAnyMenu(*toolOption)) {
            const int reserve = qMax(
                20,
                pixelMetric(
                    PM_MenuButtonIndicator,
                    toolOption,
                    widget) + 6);
            QRect logical = labelRect.adjusted(
                0,
                0,
                -reserve,
                0);
            labelRect = QStyle::visualRect(
                toolOption->direction,
                toolOption->rect,
                logical);
        }
        labelOption.rect = labelRect;

        QPalette palette = labelOption.palette;
        const QColor labelColor =
            enabled
                ? m_spec.labelColor
                : m_spec.disabledLabelColor;
        palette.setColor(
            QPalette::ButtonText,
            labelColor);
        palette.setColor(
            QPalette::WindowText,
            labelColor);
        palette.setColor(
            QPalette::Text,
            labelColor);
        labelOption.palette = palette;
        if (m_spec.hasResolvedLabelFont) {
            labelOption.fontMetrics =
                QFontMetrics(m_spec.labelFont);
        }

        painter->save();
        if (m_spec.hasResolvedLabelFont) {
            painter->setFont(m_spec.labelFont);
        }
        QProxyStyle::drawControl(
            CE_ToolButtonLabel,
            &labelOption,
            painter,
            widget);
        painter->restore();

        if (hasAnyMenu(*toolOption)) {
            paintMenuIndicator(
                painter,
                *toolOption,
                widget,
                enabled);
        }

        if (enabled
            && (toolOption->state & QStyle::State_HasFocus)
            && m_spec.focusRingWidth > 0.0) {
            painter->save();
            painter->setRenderHint(
                QPainter::Antialiasing,
                true);
            QPen focusPen(
                m_spec.focusRingColor,
                m_spec.focusRingWidth);
            focusPen.setJoinStyle(Qt::RoundJoin);
            painter->setPen(focusPen);
            painter->setBrush(Qt::NoBrush);
            painter->drawPath(path);
            painter->restore();
        }
    }

private:
    void paintMenuIndicator(
        QPainter* painter,
        const QStyleOptionToolButton& option,
        const QWidget* widget,
        bool enabled) const
    {
        QRect indicatorRect;
        if (hasSplitMenu(option)) {
            indicatorRect = subControlRect(
                CC_ToolButton,
                &option,
                SC_ToolButtonMenu,
                widget);
        } else {
            const int width = qMax(
                20,
                pixelMetric(
                    PM_MenuButtonIndicator,
                    &option,
                    widget) + 6);
            const QRect logical(
                option.rect.right() - width + 1,
                option.rect.top(),
                width,
                option.rect.height());
            indicatorRect = QStyle::visualRect(
                option.direction,
                option.rect,
                logical);
        }

        if (indicatorRect.isEmpty()) {
            return;
        }

        const QColor color =
            enabled
                ? m_spec.iconColor
                : m_spec.disabledLabelColor;
        const QPointF center = indicatorRect.center();

        painter->save();
        painter->setRenderHint(
            QPainter::Antialiasing,
            true);
        QPen pen(
            color,
            1.8,
            Qt::SolidLine,
            Qt::RoundCap,
            Qt::RoundJoin);
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);
        painter->drawLine(
            QPointF(center.x() - 4.0, center.y() - 2.0),
            QPointF(center.x(), center.y() + 2.0));
        painter->drawLine(
            QPointF(center.x(), center.y() + 2.0),
            QPointF(center.x() + 4.0, center.y() - 2.0));
        painter->restore();
    }

    ButtonSpec m_spec;
};

class ToolButtonAdapterState final : public QObject
{
public:
    explicit ToolButtonAdapterState(QToolButton* button)
        : QObject(button)
        , m_button(button)
        , m_previousStyle(
              button ? button->style() : nullptr)
        , m_themeBinding(
              new QtMaterialThemeContextBinding(
                  button,
                  this))
        , m_style(
              new NativeToolButtonProxyStyle(
                  resolvedSpec()))
    {
        setObjectName(
            QString::fromLatin1(kStateObjectName));
        m_style->setParent(this);

        if (m_button) {
            m_button->installEventFilter(this);
            m_button->setStyle(m_style);
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
        if (!m_button) {
            return;
        }

        m_button->removeEventFilter(this);
        if (m_button->style() == m_style) {
            QStyle* restoreStyle =
                m_previousStyle.data();
            if (!restoreStyle) {
                restoreStyle =
                    QApplication::style();
            }
            m_button->setStyle(restoreStyle);
        }
    }

protected:
    bool eventFilter(
        QObject* watched,
        QEvent* event) override
    {
        if (watched == m_button
            && event->type()
                   == QEvent::DynamicPropertyChange) {
            const auto* dynamic =
                static_cast<
                    QDynamicPropertyChangeEvent*>(
                    event);
            const QByteArray name =
                dynamic->propertyName();
            if (name == kVariantProperty
                || name == kDensityProperty
                || name == kAppliedProperty) {
                refreshResolvedSpec();
            }
        }
        return QObject::eventFilter(
            watched,
            event);
    }

private:
    ButtonSpec resolvedSpec() const
    {
        Q_ASSERT(m_button);
        Q_ASSERT(m_themeBinding);
        return ButtonSpecResolution::buttonSpec(
            m_themeBinding,
            QtMaterialToolButtonAdapter::variant(
                m_button),
            QtMaterialToolButtonAdapter::density(
                m_button));
    }

    void refreshResolvedSpec()
    {
        if (!m_button
            || !m_style
            || !m_themeBinding) {
            return;
        }

        m_style->setResolvedSpec(
            resolvedSpec());
        m_button->updateGeometry();
        m_button->update();
    }

    QPointer<QToolButton> m_button;
    QPointer<QStyle> m_previousStyle;
    QtMaterialThemeContextBinding*
        m_themeBinding = nullptr;
    NativeToolButtonProxyStyle* m_style = nullptr;
};

ToolButtonAdapterState* adapterState(QToolButton* button)
{
    if (!button) {
        return nullptr;
    }

    QObject* object =
        button->findChild<QObject*>(
            QString::fromLatin1(kStateObjectName),
            Qt::FindDirectChildrenOnly);
    return static_cast<ToolButtonAdapterState*>(
        object);
}

} // namespace

void QtMaterialToolButtonAdapter::apply(
    QToolButton* button,
    ButtonVariant variantValue,
    Density densityValue)
{
    if (!button) {
        return;
    }

    button->setProperty(
        kAppliedProperty,
        true);
    button->setProperty(
        kVariantProperty,
        variantName(variantValue));
    button->setProperty(
        kDensityProperty,
        densityName(densityValue));

    if (!adapterState(button)) {
        new ToolButtonAdapterState(button);
    }

    button->updateGeometry();
    button->update();
}

void QtMaterialToolButtonAdapter::remove(
    QToolButton* button)
{
    if (!button) {
        return;
    }

    if (ToolButtonAdapterState* state =
            adapterState(button)) {
        state->restore();
        delete state;
    }

    button->setProperty(
        kAppliedProperty,
        QVariant());
    button->setProperty(
        kVariantProperty,
        QVariant());
    button->setProperty(
        kDensityProperty,
        QVariant());
    button->updateGeometry();
    button->update();
}

bool QtMaterialToolButtonAdapter::isApplied(
    const QToolButton* button)
{
    return button
        && button->property(
                       kAppliedProperty)
               .toBool();
}

void QtMaterialToolButtonAdapter::setVariant(
    QToolButton* button,
    ButtonVariant variantValue)
{
    if (!button) {
        return;
    }

    if (!isApplied(button)) {
        apply(
            button,
            variantValue,
            Density::Default);
        return;
    }

    button->setProperty(
        kVariantProperty,
        variantName(variantValue));
}

ButtonVariant QtMaterialToolButtonAdapter::variant(
    const QToolButton* button)
{
    if (!button) {
        return ButtonVariant::Text;
    }
    return variantFromProperty(
        button->property(kVariantProperty));
}

void QtMaterialToolButtonAdapter::setDensity(
    QToolButton* button,
    Density densityValue)
{
    if (!button) {
        return;
    }

    if (!isApplied(button)) {
        apply(
            button,
            ButtonVariant::Text,
            densityValue);
        return;
    }

    button->setProperty(
        kDensityProperty,
        densityName(densityValue));
}

Density QtMaterialToolButtonAdapter::density(
    const QToolButton* button)
{
    if (!button) {
        return Density::Default;
    }
    return densityFromProperty(
        button->property(kDensityProperty));
}

void QtMaterialToolButtonAdapter::setOptOut(
    QToolButton* button,
    bool excluded)
{
    if (!button) {
        return;
    }

    if (excluded) {
        remove(button);
    }
    button->setProperty(
        kOptOutProperty,
        excluded);
}

bool QtMaterialToolButtonAdapter::isOptedOut(
    const QToolButton* button)
{
    return button
        && button->property(
                       kOptOutProperty)
               .toBool();
}

int QtMaterialToolButtonAdapter::applyToDescendants(
    QWidget* root,
    ButtonVariant variantValue,
    Density densityValue)
{
    if (!root) {
        return 0;
    }

    int count = 0;
    if (auto* rootButton =
            qobject_cast<QToolButton*>(root)) {
        if (!isOptedOut(rootButton)) {
            apply(
                rootButton,
                variantValue,
                densityValue);
            ++count;
        }
    }

    const auto buttons =
        root->findChildren<QToolButton*>();
    for (QToolButton* button : buttons) {
        if (!button
            || isOptedOut(button)) {
            continue;
        }
        apply(
            button,
            variantValue,
            densityValue);
        ++count;
    }

    return count;
}

const char*
QtMaterialToolButtonAdapter::
    appliedPropertyName() noexcept
{
    return kAppliedProperty;
}

const char*
QtMaterialToolButtonAdapter::
    variantPropertyName() noexcept
{
    return kVariantProperty;
}

const char*
QtMaterialToolButtonAdapter::
    densityPropertyName() noexcept
{
    return kDensityProperty;
}

const char*
QtMaterialToolButtonAdapter::
    optOutPropertyName() noexcept
{
    return kOptOutProperty;
}

} // namespace QtMaterial
