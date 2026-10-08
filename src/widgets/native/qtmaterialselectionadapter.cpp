#include "qtmaterial/widgets/native/qtmaterialselectionadapter.h"

#include <QAbstractButton>
#include <QApplication>
#include <QCheckBox>
#include <QDynamicPropertyChangeEvent>
#include <QFontMetrics>
#include <QPainter>
#include <QPainterPath>
#include <QPointer>
#include <QProxyStyle>
#include <QRadioButton>
#include <QStyleOptionButton>
#include <QVariant>
#include <QWidget>

#include <algorithm>

#include "../resolution/qtmaterialselectionspecresolution_p.h"
#include "qtmaterial/core/private/qtmaterialthemecontextbinding_p.h"

namespace QtMaterial {
namespace {

constexpr char kAppliedProperty[] = "qtm3MaterialSelection";
constexpr char kDensityProperty[] = "qtm3MaterialDensity";
constexpr char kOptOutProperty[] = "qtm3MaterialOptOut";
constexpr char kStateObjectName[] = "_qtm3_native_selection_adapter_state";

enum class NativeSelectionKind
{
    Checkbox,
    RadioButton
};

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

qreal stateOpacity(
    const QStyleOptionButton& option,
    const SelectionRuntimeSpec& spec)
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

QRect logicalSlotRect(
    const QRect& bounds,
    int slotWidth)
{
    return QRect(bounds.left(), bounds.top(), slotWidth, bounds.height());
}

QRect centeredSquare(
    const QRect& bounds,
    int size)
{
    return QRect(
        bounds.center().x() - size / 2,
        bounds.center().y() - size / 2,
        size,
        size);
}

QRect visualRect(
    const QStyleOptionButton& option,
    const QRect& logical)
{
    return QStyle::visualRect(
        option.direction,
        option.rect,
        logical);
}

QRect indicatorSlot(
    const QStyleOptionButton& option,
    const QSize& touchTarget,
    int stateLayerSize)
{
    const int width = qMax(touchTarget.width(), stateLayerSize);
    return visualRect(
        option,
        logicalSlotRect(option.rect, width));
}

QRect labelRect(
    const QStyleOptionButton& option,
    const QSize& touchTarget,
    int stateLayerSize,
    int spacing)
{
    const int slotWidth = qMax(touchTarget.width(), stateLayerSize);
    QRect logical = option.rect.adjusted(
        slotWidth + spacing,
        0,
        0,
        0);
    if (logical.width() < 0) {
        logical.setWidth(0);
    }
    return visualRect(option, logical);
}

void paintStateLayer(
    QPainter* painter,
    const QRectF& rect,
    const QColor& color,
    qreal opacity)
{
    if (!painter || opacity <= 0.0) {
        return;
    }
    QColor layer = color;
    layer.setAlphaF(
        qBound<qreal>(
            0.0,
            layer.alphaF() * opacity,
            1.0));
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);
    painter->setBrush(layer);
    painter->drawEllipse(rect);
    painter->restore();
}

void paintFocusRing(
    QPainter* painter,
    const QRectF& stateRect,
    const QColor& color)
{
    if (!painter) {
        return;
    }
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    QPen pen(color, 2.0);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawEllipse(stateRect.adjusted(-1.0, -1.0, 1.0, 1.0));
    painter->restore();
}

void paintCheckboxIndicator(
    QPainter* painter,
    const QStyleOptionButton& option,
    const CheckboxSpec& spec)
{
    const QRect slot = indicatorSlot(
        option,
        spec.touchTarget,
        spec.stateLayerSize);
    const QRect indicator = centeredSquare(slot, spec.indicatorSize);
    const QRectF stateRect(
        slot.center().x() - spec.stateLayerSize / 2.0,
        slot.center().y() - spec.stateLayerSize / 2.0,
        spec.stateLayerSize,
        spec.stateLayerSize);

    const bool enabled = option.state & QStyle::State_Enabled;
    const bool checked = option.state & QStyle::State_On;
    const bool partial = option.state & QStyle::State_NoChange;
    const bool selected = checked || partial;

    paintStateLayer(
        painter,
        stateRect,
        spec.stateLayerColor,
        stateOpacity(option, spec));

    const QColor fill = selected
        ? (enabled
               ? spec.selectedContainerColor
               : spec.disabledSelectedContainerColor)
        : QColor(Qt::transparent);
    const QColor outline = selected
        ? fill
        : (enabled
               ? spec.unselectedOutlineColor
               : spec.disabledUnselectedOutlineColor);

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(QPen(outline, spec.outlineWidth));
    painter->setBrush(fill);
    painter->drawRoundedRect(
        QRectF(indicator),
        spec.cornerRadius,
        spec.cornerRadius);

    if (selected) {
        QColor mark = spec.selectedIconColor;
        if (!enabled) {
            mark.setAlphaF(mark.alphaF() * 0.72);
        }
        QPen markPen(
            mark,
            spec.checkmarkStrokeWidth,
            Qt::SolidLine,
            Qt::RoundCap,
            Qt::RoundJoin);
        painter->setPen(markPen);

        if (partial) {
            const qreal y = indicator.center().y();
            painter->drawLine(
                QPointF(
                    indicator.left() + indicator.width() * 0.24,
                    y),
                QPointF(
                    indicator.right() - indicator.width() * 0.24,
                    y));
        } else {
            const QPointF p1(
                indicator.left() + indicator.width() * 0.22,
                indicator.top() + indicator.height() * 0.55);
            const QPointF p2(
                indicator.left() + indicator.width() * 0.45,
                indicator.bottom() - indicator.height() * 0.22);
            const QPointF p3(
                indicator.right() - indicator.width() * 0.18,
                indicator.top() + indicator.height() * 0.22);
            painter->drawLine(p1, p2);
            painter->drawLine(p2, p3);
        }
    }
    painter->restore();

    if (enabled && (option.state & QStyle::State_HasFocus)) {
        paintFocusRing(painter, stateRect, spec.focusRingColor);
    }
}

void paintRadioIndicator(
    QPainter* painter,
    const QStyleOptionButton& option,
    const RadioButtonSpec& spec)
{
    const QRect slot = indicatorSlot(
        option,
        spec.touchTarget,
        spec.stateLayerSize);
    const QRect indicator = centeredSquare(slot, spec.indicatorSize);
    const QRectF stateRect(
        slot.center().x() - spec.stateLayerSize / 2.0,
        slot.center().y() - spec.stateLayerSize / 2.0,
        spec.stateLayerSize,
        spec.stateLayerSize);

    const bool enabled = option.state & QStyle::State_Enabled;
    const bool checked = option.state & QStyle::State_On;

    paintStateLayer(
        painter,
        stateRect,
        spec.stateLayerColor,
        stateOpacity(option, spec));

    const QColor indicatorColor = enabled
        ? (checked
               ? spec.selectedColor
               : spec.unselectedOutlineColor)
        : spec.disabledColor;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(QPen(indicatorColor, spec.outlineWidth));
    painter->setBrush(Qt::NoBrush);
    painter->drawEllipse(QRectF(indicator));

    if (checked) {
        const QRect dot = centeredSquare(
            indicator,
            spec.dotSize);
        painter->setPen(Qt::NoPen);
        painter->setBrush(indicatorColor);
        painter->drawEllipse(QRectF(dot));
    }
    painter->restore();

    if (enabled && (option.state & QStyle::State_HasFocus)) {
        paintFocusRing(painter, stateRect, spec.focusRingColor);
    }
}

class NativeSelectionProxyStyle final : public QProxyStyle
{
public:
    explicit NativeSelectionProxyStyle(
        NativeSelectionKind kind,
        const CheckboxSpec& checkboxSpec,
        const RadioButtonSpec& radioSpec)
        : QProxyStyle()
        , m_kind(kind)
        , m_checkboxSpec(checkboxSpec)
        , m_radioSpec(radioSpec)
    {
    }

    void setCheckboxSpec(const CheckboxSpec& spec)
    {
        m_checkboxSpec = spec;
    }

    void setRadioSpec(const RadioButtonSpec& spec)
    {
        m_radioSpec = spec;
    }

    void drawControl(
        ControlElement element,
        const QStyleOption* option,
        QPainter* painter,
        const QWidget* widget = nullptr) const override
    {
        const auto* buttonOption =
            qstyleoption_cast<const QStyleOptionButton*>(option);
        if (!buttonOption || !widget) {
            QProxyStyle::drawControl(
                element, option, painter, widget);
            return;
        }

        if (m_kind == NativeSelectionKind::Checkbox
            && element == CE_CheckBox
            && qobject_cast<const QCheckBox*>(widget)
            && QtMaterialSelectionAdapter::isApplied(
                qobject_cast<const QCheckBox*>(widget))) {
            paintCheckboxIndicator(
                painter,
                *buttonOption,
                m_checkboxSpec);
            drawCheckboxLabel(
                painter,
                *buttonOption,
                widget);
            return;
        }

        if (m_kind == NativeSelectionKind::RadioButton
            && element == CE_RadioButton
            && qobject_cast<const QRadioButton*>(widget)
            && QtMaterialSelectionAdapter::isApplied(
                qobject_cast<const QRadioButton*>(widget))) {
            paintRadioIndicator(
                painter,
                *buttonOption,
                m_radioSpec);
            drawRadioLabel(
                painter,
                *buttonOption,
                widget);
            return;
        }

        QProxyStyle::drawControl(
            element, option, painter, widget);
    }

    QSize sizeFromContents(
        ContentsType type,
        const QStyleOption* option,
        const QSize& contentsSize,
        const QWidget* widget = nullptr) const override
    {
        const auto* buttonOption =
            qstyleoption_cast<const QStyleOptionButton*>(option);
        if (!buttonOption || !widget) {
            return QProxyStyle::sizeFromContents(
                type, option, contentsSize, widget);
        }

        if (m_kind == NativeSelectionKind::Checkbox
            && type == CT_CheckBox
            && qobject_cast<const QCheckBox*>(widget)) {
            return selectionSize(
                *buttonOption,
                m_checkboxSpec.touchTarget,
                m_checkboxSpec.stateLayerSize,
                m_checkboxSpec.spacing);
        }

        if (m_kind == NativeSelectionKind::RadioButton
            && type == CT_RadioButton
            && qobject_cast<const QRadioButton*>(widget)) {
            return selectionSize(
                *buttonOption,
                m_radioSpec.touchTarget,
                m_radioSpec.stateLayerSize,
                m_radioSpec.spacing);
        }

        return QProxyStyle::sizeFromContents(
            type, option, contentsSize, widget);
    }

private:
    static QSize selectionSize(
        const QStyleOptionButton& option,
        const QSize& touchTarget,
        int stateLayerSize,
        int spacing)
    {
        int labelWidth =
            option.fontMetrics.horizontalAdvance(option.text);
        int labelHeight = option.fontMetrics.height();

        if (!option.icon.isNull()) {
            const QSize iconSize = option.iconSize.isValid()
                ? option.iconSize
                : QSize(16, 16);
            labelWidth += iconSize.width();
            labelHeight = qMax(labelHeight, iconSize.height());
            if (!option.text.isEmpty()) {
                labelWidth += 4;
            }
        }

        const int slotWidth =
            qMax(touchTarget.width(), stateLayerSize);
        const int width =
            slotWidth
            + (labelWidth > 0 ? spacing + labelWidth : 0);
        const int height = qMax(
            touchTarget.height(),
            labelHeight);
        return QSize(width, height);
    }

    void drawCheckboxLabel(
        QPainter* painter,
        const QStyleOptionButton& option,
        const QWidget* widget) const
    {
        QStyleOptionButton labelOption(option);
        labelOption.rect = labelRect(
            option,
            m_checkboxSpec.touchTarget,
            m_checkboxSpec.stateLayerSize,
            m_checkboxSpec.spacing);
        QPalette palette = labelOption.palette;
        const QColor color =
            (option.state & QStyle::State_Enabled)
                ? m_checkboxSpec.labelColor
                : m_checkboxSpec.disabledLabelColor;
        palette.setColor(QPalette::WindowText, color);
        palette.setColor(QPalette::ButtonText, color);
        labelOption.palette = palette;

        painter->save();
        if (m_checkboxSpec.hasResolvedLabelFont) {
            painter->setFont(m_checkboxSpec.labelFont);
            labelOption.fontMetrics =
                QFontMetrics(m_checkboxSpec.labelFont);
        }
        QProxyStyle::drawControl(
            CE_CheckBoxLabel,
            &labelOption,
            painter,
            widget);
        painter->restore();
    }

    void drawRadioLabel(
        QPainter* painter,
        const QStyleOptionButton& option,
        const QWidget* widget) const
    {
        QStyleOptionButton labelOption(option);
        labelOption.rect = labelRect(
            option,
            m_radioSpec.touchTarget,
            m_radioSpec.stateLayerSize,
            m_radioSpec.spacing);
        QPalette palette = labelOption.palette;
        const QColor color =
            (option.state & QStyle::State_Enabled)
                ? m_radioSpec.labelColor
                : m_radioSpec.disabledLabelColor;
        palette.setColor(QPalette::WindowText, color);
        palette.setColor(QPalette::ButtonText, color);
        labelOption.palette = palette;

        painter->save();
        if (m_radioSpec.hasResolvedLabelFont) {
            painter->setFont(m_radioSpec.labelFont);
            labelOption.fontMetrics =
                QFontMetrics(m_radioSpec.labelFont);
        }
        QProxyStyle::drawControl(
            CE_RadioButtonLabel,
            &labelOption,
            painter,
            widget);
        painter->restore();
    }

    NativeSelectionKind m_kind;
    CheckboxSpec m_checkboxSpec;
    RadioButtonSpec m_radioSpec;
};

class SelectionAdapterState final : public QObject
{
public:
    SelectionAdapterState(
        QAbstractButton* button,
        NativeSelectionKind kind)
        : QObject(button)
        , m_button(button)
        , m_kind(kind)
        , m_previousStyle(button ? button->style() : nullptr)
        , m_themeBinding(
              new QtMaterialThemeContextBinding(button, this))
        , m_style(new NativeSelectionProxyStyle(
              kind,
              checkboxSpec(),
              radioSpec()))
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
            [this]() { refreshResolvedSpecs(); });
        QObject::connect(
            m_themeBinding,
            &QtMaterialThemeContextBinding::effectiveThemeContextChanged,
            this,
            [this]() { refreshResolvedSpecs(); });
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
            if (name == kDensityProperty
                || name == kAppliedProperty) {
                refreshResolvedSpecs();
            }
        }
        return QObject::eventFilter(watched, event);
    }

private:
    Density currentDensity() const
    {
        if (!m_button) {
            return Density::Default;
        }
        return densityFromProperty(
            m_button->property(kDensityProperty));
    }

    CheckboxSpec checkboxSpec() const
    {
        if (!m_themeBinding) {
            return CheckboxSpec();
        }
        return SelectionSpecResolution::checkboxSpec(
            m_themeBinding,
            currentDensity());
    }

    RadioButtonSpec radioSpec() const
    {
        if (!m_themeBinding) {
            return RadioButtonSpec();
        }
        return SelectionSpecResolution::radioButtonSpec(
            m_themeBinding,
            currentDensity());
    }

    void refreshResolvedSpecs()
    {
        if (!m_button || !m_style || !m_themeBinding) {
            return;
        }

        if (m_kind == NativeSelectionKind::Checkbox) {
            m_style->setCheckboxSpec(checkboxSpec());
        } else {
            m_style->setRadioSpec(radioSpec());
        }

        m_button->updateGeometry();
        m_button->update();
    }

    QPointer<QAbstractButton> m_button;
    NativeSelectionKind m_kind;
    QPointer<QStyle> m_previousStyle;
    QtMaterialThemeContextBinding* m_themeBinding = nullptr;
    NativeSelectionProxyStyle* m_style = nullptr;
};

SelectionAdapterState* adapterState(QAbstractButton* button)
{
    if (!button) {
        return nullptr;
    }
    QObject* object = button->findChild<QObject*>(
        QString::fromLatin1(kStateObjectName),
        Qt::FindDirectChildrenOnly);
    return static_cast<SelectionAdapterState*>(object);
}

void applyImpl(
    QAbstractButton* button,
    NativeSelectionKind kind,
    Density density)
{
    if (!button) {
        return;
    }

    button->setProperty(kAppliedProperty, true);
    button->setProperty(kDensityProperty, densityName(density));

    if (!adapterState(button)) {
        new SelectionAdapterState(button, kind);
    }

    button->updateGeometry();
    button->update();
}

void removeImpl(QAbstractButton* button)
{
    if (!button) {
        return;
    }

    if (SelectionAdapterState* state = adapterState(button)) {
        state->restore();
        delete state;
    }

    button->setProperty(kAppliedProperty, QVariant());
    button->setProperty(kDensityProperty, QVariant());
    button->updateGeometry();
    button->update();
}

bool isAppliedImpl(const QAbstractButton* button)
{
    return button && button->property(kAppliedProperty).toBool();
}

void setDensityImpl(
    QAbstractButton* button,
    NativeSelectionKind kind,
    Density density)
{
    if (!button) {
        return;
    }
    if (!isAppliedImpl(button)) {
        applyImpl(button, kind, density);
        return;
    }
    button->setProperty(kDensityProperty, densityName(density));
}

Density densityImpl(const QAbstractButton* button)
{
    if (!button) {
        return Density::Default;
    }
    return densityFromProperty(
        button->property(kDensityProperty));
}

void setOptOutImpl(
    QAbstractButton* button,
    bool excluded)
{
    if (!button) {
        return;
    }
    if (excluded) {
        removeImpl(button);
    }
    button->setProperty(kOptOutProperty, excluded);
}

bool isOptedOutImpl(const QAbstractButton* button)
{
    return button && button->property(kOptOutProperty).toBool();
}

} // namespace

void QtMaterialSelectionAdapter::apply(
    QCheckBox* checkbox,
    Density densityValue)
{
    applyImpl(
        checkbox,
        NativeSelectionKind::Checkbox,
        densityValue);
}

void QtMaterialSelectionAdapter::apply(
    QRadioButton* radio,
    Density densityValue)
{
    applyImpl(
        radio,
        NativeSelectionKind::RadioButton,
        densityValue);
}

void QtMaterialSelectionAdapter::remove(QCheckBox* checkbox)
{
    removeImpl(checkbox);
}

void QtMaterialSelectionAdapter::remove(QRadioButton* radio)
{
    removeImpl(radio);
}

bool QtMaterialSelectionAdapter::isApplied(
    const QCheckBox* checkbox)
{
    return isAppliedImpl(checkbox);
}

bool QtMaterialSelectionAdapter::isApplied(
    const QRadioButton* radio)
{
    return isAppliedImpl(radio);
}

void QtMaterialSelectionAdapter::setDensity(
    QCheckBox* checkbox,
    Density densityValue)
{
    setDensityImpl(
        checkbox,
        NativeSelectionKind::Checkbox,
        densityValue);
}

void QtMaterialSelectionAdapter::setDensity(
    QRadioButton* radio,
    Density densityValue)
{
    setDensityImpl(
        radio,
        NativeSelectionKind::RadioButton,
        densityValue);
}

Density QtMaterialSelectionAdapter::density(
    const QCheckBox* checkbox)
{
    return densityImpl(checkbox);
}

Density QtMaterialSelectionAdapter::density(
    const QRadioButton* radio)
{
    return densityImpl(radio);
}

void QtMaterialSelectionAdapter::setOptOut(
    QCheckBox* checkbox,
    bool excluded)
{
    setOptOutImpl(checkbox, excluded);
}

void QtMaterialSelectionAdapter::setOptOut(
    QRadioButton* radio,
    bool excluded)
{
    setOptOutImpl(radio, excluded);
}

bool QtMaterialSelectionAdapter::isOptedOut(
    const QCheckBox* checkbox)
{
    return isOptedOutImpl(checkbox);
}

bool QtMaterialSelectionAdapter::isOptedOut(
    const QRadioButton* radio)
{
    return isOptedOutImpl(radio);
}

int QtMaterialSelectionAdapter::applyToDescendants(
    QWidget* root,
    Density densityValue)
{
    if (!root) {
        return 0;
    }

    int count = 0;

    if (auto* checkbox = qobject_cast<QCheckBox*>(root)) {
        if (!isOptedOut(checkbox)) {
            apply(checkbox, densityValue);
            ++count;
        }
    } else if (auto* radio = qobject_cast<QRadioButton*>(root)) {
        if (!isOptedOut(radio)) {
            apply(radio, densityValue);
            ++count;
        }
    }

    const auto checkboxes = root->findChildren<QCheckBox*>();
    for (QCheckBox* checkbox : checkboxes) {
        if (!checkbox || isOptedOut(checkbox)) {
            continue;
        }
        apply(checkbox, densityValue);
        ++count;
    }

    const auto radios = root->findChildren<QRadioButton*>();
    for (QRadioButton* radio : radios) {
        if (!radio || isOptedOut(radio)) {
            continue;
        }
        apply(radio, densityValue);
        ++count;
    }

    return count;
}

const char* QtMaterialSelectionAdapter::appliedPropertyName() noexcept
{
    return kAppliedProperty;
}

const char* QtMaterialSelectionAdapter::densityPropertyName() noexcept
{
    return kDensityProperty;
}

const char* QtMaterialSelectionAdapter::optOutPropertyName() noexcept
{
    return kOptOutProperty;
}

} // namespace QtMaterial
