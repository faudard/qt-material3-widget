#include "qtmaterial/widgets/native/qtmaterialbuttonadapter.h"

#include <QApplication>
#include <QProxyStyle>
#include <QDynamicPropertyChangeEvent>
#include <QFontMetrics>
#include <QPainter>
#include <QPainterPath>
#include <QPointer>
#include <QPushButton>
#include <QStyleOptionButton>
#include <QVariant>
#include <QWidget>

#include <algorithm>

#include "../resolution/qtmaterialbuttonspecresolution_p.h"
#include "qtmaterial/core/private/qtmaterialthemecontextbinding_p.h"

namespace QtMaterial {
namespace {

constexpr char kAppliedProperty[] = "qtm3MaterialButton";
constexpr char kVariantProperty[] = "qtm3MaterialVariant";
constexpr char kDensityProperty[] = "qtm3MaterialDensity";
constexpr char kOptOutProperty[] = "qtm3MaterialOptOut";
constexpr char kStateObjectName[] = "_qtm3_native_button_adapter_state";

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
    const qreal height = qMin<qreal>(spec.containerHeight, result.height());
    result.setTop(result.center().y() - height / 2.0);
    result.setHeight(height);
    return result;
}

qreal cornerRadius(const ButtonSpec& spec, const QRectF& rect)
{
    if (spec.cornerRadius < 0.0) {
        return rect.height() / 2.0;
    }
    return qMin(spec.cornerRadius, rect.height() / 2.0);
}

qreal stateOpacity(const QStyleOptionButton& option, const ButtonSpec& spec)
{
    if (!(option.state & QStyle::State_Enabled)) {
        return 0.0;
    }
    if (option.state & QStyle::State_Sunken) {
        return spec.pressStateLayerOpacity;
    }

    qreal opacity = 0.0;
    if (option.state & QStyle::State_MouseOver) {
        opacity = qMax(opacity, spec.hoverStateLayerOpacity);
    }
    if (option.state & QStyle::State_HasFocus) {
        opacity = qMax(opacity, spec.focusStateLayerOpacity);
    }
    return opacity;
}

class NativeButtonProxyStyle final : public QProxyStyle
{
public:
    explicit NativeButtonProxyStyle(const ButtonSpec& spec)
        : QProxyStyle()
        , m_spec(spec)
    {
    }

    void setResolvedSpec(const ButtonSpec& spec)
    {
        m_spec = spec;
    }

    void drawControl(
        ControlElement element,
        const QStyleOption* option,
        QPainter* painter,
        const QWidget* widget = nullptr) const override
    {
        const auto* button = qobject_cast<const QPushButton*>(widget);
        const auto* buttonOption =
            qstyleoption_cast<const QStyleOptionButton*>(option);

        if (element != CE_PushButton
            || !button
            || !buttonOption
            || !QtMaterialButtonAdapter::isApplied(button)) {
            QProxyStyle::drawControl(
                element, option, painter, widget);
            return;
        }

        const ButtonSpec& spec = m_spec;
        const QRectF visualRect = containerRect(buttonOption->rect, spec);
        const qreal radius = cornerRadius(spec, visualRect);
        QPainterPath path;
        path.addRoundedRect(visualRect, radius, radius);

        const bool enabled =
            buttonOption->state & QStyle::State_Enabled;
        const QColor container =
            enabled ? spec.containerColor : spec.disabledContainerColor;

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(Qt::NoPen);
        painter->setBrush(container);
        painter->drawPath(path);

        const qreal layerOpacity = stateOpacity(*buttonOption, spec);
        if (layerOpacity > 0.0) {
            QColor layer = spec.stateLayerColor;
            layer.setAlphaF(
                qBound<qreal>(
                    0.0,
                    layer.alphaF() * layerOpacity,
                    1.0));
            painter->setBrush(layer);
            painter->drawPath(path);
        }

        const QColor outline =
            enabled ? spec.outlineColor : spec.disabledOutlineColor;
        if (spec.outlineWidth > 0.0 && outline.alpha() > 0) {
            QPen pen(outline, spec.outlineWidth);
            pen.setJoinStyle(Qt::RoundJoin);
            painter->setPen(pen);
            painter->setBrush(Qt::NoBrush);
            painter->drawPath(path);
        }
        painter->restore();

        QStyleOptionButton labelOption(*buttonOption);
        QPalette palette = labelOption.palette;
        const QColor label =
            enabled ? spec.labelColor : spec.disabledLabelColor;
        palette.setColor(QPalette::ButtonText, label);
        palette.setColor(QPalette::WindowText, label);
        labelOption.palette = palette;
        if (spec.hasResolvedLabelFont) {
            labelOption.fontMetrics = QFontMetrics(spec.labelFont);
        }

        QProxyStyle::drawControl(
            CE_PushButtonLabel,
            &labelOption,
            painter,
            widget);

        if (enabled
            && (buttonOption->state & QStyle::State_HasFocus)
            && spec.focusRingWidth > 0.0) {
            painter->save();
            painter->setRenderHint(QPainter::Antialiasing, true);
            QPen focusPen(spec.focusRingColor, spec.focusRingWidth);
            focusPen.setJoinStyle(Qt::RoundJoin);
            painter->setPen(focusPen);
            painter->setBrush(Qt::NoBrush);
            painter->drawPath(path);
            painter->restore();
        }
    }

    QSize sizeFromContents(
        ContentsType type,
        const QStyleOption* option,
        const QSize& contentsSize,
        const QWidget* widget = nullptr) const override
    {
        const auto* button = qobject_cast<const QPushButton*>(widget);
        const auto* buttonOption =
            qstyleoption_cast<const QStyleOptionButton*>(option);
        if (type != CT_PushButton
            || !button
            || !buttonOption
            || !QtMaterialButtonAdapter::isApplied(button)) {
            return QProxyStyle::sizeFromContents(
                type, option, contentsSize, widget);
        }

        const ButtonSpec& spec = m_spec;
        int contentWidth =
            buttonOption->fontMetrics.horizontalAdvance(buttonOption->text);
        if (!buttonOption->icon.isNull()) {
            const QSize iconSize = buttonOption->iconSize.isValid()
                ? buttonOption->iconSize
                : QSize(spec.iconSize, spec.iconSize);
            contentWidth += iconSize.width();
            if (!buttonOption->text.isEmpty()) {
                contentWidth += spec.iconSpacing;
            }
        }
        if (buttonOption->features & QStyleOptionButton::HasMenu) {
            contentWidth += pixelMetric(
                PM_MenuButtonIndicator,
                buttonOption,
                widget);
        }

        const int width = qMax(
            64,
            contentWidth + 2 * spec.horizontalPadding);
        const int height = qMax(
            spec.touchTarget.height(),
            spec.containerHeight);
        return QSize(width, height);
    }

private:
    ButtonSpec m_spec;
};

class ButtonAdapterState final : public QObject
{
public:
    explicit ButtonAdapterState(QPushButton* button)
        : QObject(button)
        , m_button(button)
        , m_previousStyle(button ? button->style() : nullptr)
        , m_themeBinding(
              new QtMaterialThemeContextBinding(button, this))
        , m_style(new NativeButtonProxyStyle(
              resolvedSpec()))
    {
        setObjectName(QString::fromLatin1(kStateObjectName));
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
            &QtMaterialThemeContextBinding::effectiveThemeContextChanged,
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
            QStyle* restoreStyle = m_previousStyle.data();
            if (!restoreStyle) {
                restoreStyle = QApplication::style();
            }
            m_button->setStyle(restoreStyle);
        }
    }

protected:
    bool eventFilter(QObject* watched, QEvent* event) override
    {
        if (watched == m_button
            && event->type() == QEvent::DynamicPropertyChange) {
            const auto* dynamic =
                static_cast<QDynamicPropertyChangeEvent*>(event);
            const QByteArray name = dynamic->propertyName();
            if (name == kVariantProperty
                || name == kDensityProperty
                || name == kAppliedProperty) {
                refreshResolvedSpec();
            }
        }
        return QObject::eventFilter(watched, event);
    }

private:
    ButtonSpec resolvedSpec() const
    {
        Q_ASSERT(m_button);
        Q_ASSERT(m_themeBinding);
        return ButtonSpecResolution::buttonSpec(
            m_themeBinding,
            QtMaterialButtonAdapter::variant(m_button),
            QtMaterialButtonAdapter::density(m_button));
    }

    void refreshResolvedSpec()
    {
        if (!m_button || !m_style || !m_themeBinding) {
            return;
        }
        m_style->setResolvedSpec(resolvedSpec());
        m_button->updateGeometry();
        m_button->update();
    }

    QPointer<QPushButton> m_button;
    QPointer<QStyle> m_previousStyle;
    QtMaterialThemeContextBinding* m_themeBinding = nullptr;
    NativeButtonProxyStyle* m_style = nullptr;
};

ButtonAdapterState* adapterState(QPushButton* button)
{
    if (!button) {
        return nullptr;
    }
    QObject* object = button->findChild<QObject*>(
        QString::fromLatin1(kStateObjectName),
        Qt::FindDirectChildrenOnly);
    return static_cast<ButtonAdapterState*>(object);
}

} // namespace

void QtMaterialButtonAdapter::apply(
    QPushButton* button,
    ButtonVariant variantValue,
    Density densityValue)
{
    if (!button) {
        return;
    }

    button->setProperty(kAppliedProperty, true);
    button->setProperty(kVariantProperty, variantName(variantValue));
    button->setProperty(kDensityProperty, densityName(densityValue));

    if (!adapterState(button)) {
        new ButtonAdapterState(button);
    }

    button->updateGeometry();
    button->update();
}

void QtMaterialButtonAdapter::remove(QPushButton* button)
{
    if (!button) {
        return;
    }

    if (ButtonAdapterState* state = adapterState(button)) {
        state->restore();
        delete state;
    }

    button->setProperty(kAppliedProperty, QVariant());
    button->setProperty(kVariantProperty, QVariant());
    button->setProperty(kDensityProperty, QVariant());
    button->updateGeometry();
    button->update();
}

bool QtMaterialButtonAdapter::isApplied(const QPushButton* button)
{
    return button && button->property(kAppliedProperty).toBool();
}

void QtMaterialButtonAdapter::setVariant(
    QPushButton* button,
    ButtonVariant variantValue)
{
    if (!button) {
        return;
    }
    if (!isApplied(button)) {
        apply(button, variantValue, density(button));
        return;
    }
    button->setProperty(kVariantProperty, variantName(variantValue));
}

ButtonVariant QtMaterialButtonAdapter::variant(const QPushButton* button)
{
    if (!button) {
        return ButtonVariant::Text;
    }
    return variantFromProperty(button->property(kVariantProperty));
}

void QtMaterialButtonAdapter::setDensity(
    QPushButton* button,
    Density densityValue)
{
    if (!button) {
        return;
    }
    if (!isApplied(button)) {
        apply(button, variant(button), densityValue);
        return;
    }
    button->setProperty(kDensityProperty, densityName(densityValue));
}

Density QtMaterialButtonAdapter::density(const QPushButton* button)
{
    if (!button) {
        return Density::Default;
    }
    return densityFromProperty(button->property(kDensityProperty));
}

void QtMaterialButtonAdapter::setOptOut(
    QPushButton* button,
    bool excluded)
{
    if (!button) {
        return;
    }
    if (excluded) {
        remove(button);
    }
    button->setProperty(kOptOutProperty, excluded);
}

bool QtMaterialButtonAdapter::isOptedOut(const QPushButton* button)
{
    return button && button->property(kOptOutProperty).toBool();
}

int QtMaterialButtonAdapter::applyToDescendants(
    QWidget* root,
    ButtonVariant variantValue,
    Density densityValue)
{
    if (!root) {
        return 0;
    }

    int count = 0;
    if (auto* rootButton = qobject_cast<QPushButton*>(root)) {
        if (!isOptedOut(rootButton)) {
            apply(rootButton, variantValue, densityValue);
            ++count;
        }
    }

    const auto buttons = root->findChildren<QPushButton*>();
    for (QPushButton* button : buttons) {
        if (!button || isOptedOut(button)) {
            continue;
        }
        apply(button, variantValue, densityValue);
        ++count;
    }
    return count;
}

const char* QtMaterialButtonAdapter::appliedPropertyName() noexcept
{
    return kAppliedProperty;
}

const char* QtMaterialButtonAdapter::variantPropertyName() noexcept
{
    return kVariantProperty;
}

const char* QtMaterialButtonAdapter::densityPropertyName() noexcept
{
    return kDensityProperty;
}

const char* QtMaterialButtonAdapter::optOutPropertyName() noexcept
{
    return kOptOutProperty;
}

} // namespace QtMaterial
