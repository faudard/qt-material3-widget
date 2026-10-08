#include "qtmaterial/widgets/native/qtmaterialcomboboxadapter.h"

#include <QApplication>
#include <QComboBox>
#include <QDynamicPropertyChangeEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPointer>
#include <QProxyStyle>
#include <QStyleOptionComboBox>
#include <QVariant>
#include <QtMath>
#include <QWidget>

#include "../resolution/qtmaterialinputspecresolution_p.h"
#include "qtmaterial/core/private/qtmaterialthemecontextbinding_p.h"

namespace QtMaterial {
namespace {

constexpr char kAppliedProperty[] = "qtm3MaterialComboBox";
constexpr char kDensityProperty[] = "qtm3MaterialDensity";
constexpr char kOptOutProperty[] = "qtm3MaterialOptOut";
constexpr char kStateObjectName[] = "_qtm3_native_combobox_adapter_state";

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

QColor withOpacity(QColor color, qreal opacity)
{
    color.setAlphaF(
        qBound<qreal>(
            0.0,
            color.alphaF() * opacity,
            1.0));
    return color;
}

int comboMinimumHeight(const AutocompleteSpec& spec)
{
    return qMax(40, spec.inputMinHeight - 8);
}

int arrowAreaWidth(const AutocompleteSpec& spec)
{
    return qMax(32, spec.horizontalPadding + 18);
}

class NativeComboBoxProxyStyle final : public QProxyStyle
{
public:
    explicit NativeComboBoxProxyStyle(
        const AutocompleteSpec& spec)
        : QProxyStyle()
        , m_spec(spec)
    {
    }

    void setResolvedSpec(const AutocompleteSpec& spec)
    {
        m_spec = spec;
    }

    QRect subControlRect(
        ComplexControl control,
        const QStyleOptionComplex* option,
        SubControl subControl,
        const QWidget* widget = nullptr) const override
    {
        const auto* combo =
            qobject_cast<const QComboBox*>(widget);
        const auto* comboOption =
            qstyleoption_cast<const QStyleOptionComboBox*>(option);

        if (control != CC_ComboBox
            || !combo
            || !comboOption
            || !QtMaterialComboBoxAdapter::isApplied(combo)) {
            return QProxyStyle::subControlRect(
                control,
                option,
                subControl,
                widget);
        }

        const QRect bounds = comboOption->rect;
        const int arrowWidth = arrowAreaWidth(m_spec);
        const int padding = qMax(8, m_spec.horizontalPadding);

        if (subControl == SC_ComboBoxFrame) {
            return bounds;
        }

        if (subControl == SC_ComboBoxArrow) {
            const QRect logical(
                bounds.right() - arrowWidth + 1,
                bounds.top(),
                arrowWidth,
                bounds.height());
            return QStyle::visualRect(
                comboOption->direction,
                bounds,
                logical);
        }

        if (subControl == SC_ComboBoxEditField) {
            QRect logical = bounds.adjusted(
                padding,
                0,
                -(arrowWidth + qMax(4, padding / 2)),
                0);
            if (logical.width() < 0) {
                logical.setWidth(0);
            }
            return QStyle::visualRect(
                comboOption->direction,
                bounds,
                logical);
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
        const auto* combo =
            qobject_cast<const QComboBox*>(widget);
        const auto* comboOption =
            qstyleoption_cast<const QStyleOptionComboBox*>(option);

        if (control == CC_ComboBox
            && combo
            && comboOption
            && QtMaterialComboBoxAdapter::isApplied(combo)) {
            if (subControlRect(
                    control,
                    option,
                    SC_ComboBoxArrow,
                    widget)
                    .contains(position)) {
                return SC_ComboBoxArrow;
            }

            if (subControlRect(
                    control,
                    option,
                    SC_ComboBoxEditField,
                    widget)
                    .contains(position)) {
                return SC_ComboBoxEditField;
            }
            return SC_ComboBoxFrame;
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

        const auto* combo =
            qobject_cast<const QComboBox*>(widget);
        if (type != CT_ComboBox
            || !combo
            || !QtMaterialComboBoxAdapter::isApplied(combo)) {
            return size;
        }

        size.setHeight(
            qMax(
                size.height(),
                comboMinimumHeight(m_spec)));
        size.rwidth() += 8;
        return size;
    }

    int pixelMetric(
        PixelMetric metric,
        const QStyleOption* option = nullptr,
        const QWidget* widget = nullptr) const override
    {
        const auto* combo =
            qobject_cast<const QComboBox*>(widget);
        if (combo
            && QtMaterialComboBoxAdapter::isApplied(combo)
            && metric == PM_ComboBoxFrameWidth) {
            return qMax(
                1,
                qCeil(m_spec.outlineWidth));
        }

        return QProxyStyle::pixelMetric(
            metric,
            option,
            widget);
    }

    void drawComplexControl(
        ComplexControl control,
        const QStyleOptionComplex* option,
        QPainter* painter,
        const QWidget* widget = nullptr) const override
    {
        const auto* combo =
            qobject_cast<const QComboBox*>(widget);
        const auto* comboOption =
            qstyleoption_cast<const QStyleOptionComboBox*>(option);

        if (control != CC_ComboBox
            || !combo
            || !comboOption
            || !QtMaterialComboBoxAdapter::isApplied(combo)) {
            QProxyStyle::drawComplexControl(
                control,
                option,
                painter,
                widget);
            return;
        }

        const QRectF fieldRect =
            QRectF(comboOption->rect).adjusted(
                0.5,
                0.5,
                -0.5,
                -0.5);
        if (!fieldRect.isValid()) {
            return;
        }

        const qreal radius =
            qMin<qreal>(
                qMax<qreal>(
                    0.0,
                    m_spec.inputCornerRadius),
                fieldRect.height() / 2.0);

        QPainterPath path;
        path.addRoundedRect(
            fieldRect,
            radius,
            radius);

        const bool enabled =
            comboOption->state & QStyle::State_Enabled;
        const bool focused =
            comboOption->state & QStyle::State_HasFocus;
        const bool pressed =
            comboOption->state & QStyle::State_Sunken;
        const bool hovered =
            comboOption->state & QStyle::State_MouseOver;

        painter->save();
        painter->setRenderHint(
            QPainter::Antialiasing,
            true);
        painter->fillPath(
            path,
            m_spec.inputContainerColor);

        if (enabled && (pressed || hovered)) {
            painter->fillPath(
                path,
                withOpacity(
                    m_spec.stateLayerColor,
                    pressed
                        ? m_spec.pressStateLayerOpacity
                        : m_spec.hoverStateLayerOpacity));
        }

        QColor outline =
            focused
                ? m_spec.focusedOutlineColor
                : m_spec.outlineColor;
        if (!enabled) {
            outline.setAlphaF(
                outline.alphaF() * 0.45);
        }

        painter->setBrush(Qt::NoBrush);
        painter->setPen(
            QPen(
                outline,
                focused
                    ? m_spec.focusedOutlineWidth
                    : m_spec.outlineWidth));
        painter->drawPath(path);

        if (enabled
            && focused
            && m_spec.focusRingWidth > 0.0) {
            painter->setPen(
                QPen(
                    m_spec.focusRingColor,
                    m_spec.focusRingWidth));
            painter->drawPath(
                path.translated(0.0, 0.0));
        }
        painter->restore();

        if (!comboOption->editable) {
            QStyleOptionComboBox labelOption(*comboOption);
            labelOption.rect = subControlRect(
                CC_ComboBox,
                comboOption,
                SC_ComboBoxEditField,
                widget);

            QPalette palette = labelOption.palette;
            const QColor textColor =
                enabled
                    ? m_spec.inputTextColor
                    : m_spec.disabledTextColor;
            palette.setColor(
                QPalette::ButtonText,
                textColor);
            palette.setColor(
                QPalette::WindowText,
                textColor);
            palette.setColor(
                QPalette::Text,
                textColor);
            labelOption.palette = palette;

            painter->save();
            if (m_spec.hasResolvedInputFont) {
                painter->setFont(m_spec.inputFont);
            }
            QProxyStyle::drawControl(
                CE_ComboBoxLabel,
                &labelOption,
                painter,
                widget);
            painter->restore();
        }

        if (comboOption->subControls & SC_ComboBoxArrow) {
            const QRect arrow = subControlRect(
                CC_ComboBox,
                comboOption,
                SC_ComboBoxArrow,
                widget);
            const QPointF center = arrow.center();
            const qreal halfWidth = 5.0;
            const qreal halfHeight = 2.5;

            const QColor arrowColor =
                enabled
                    ? m_spec.inputTextColor
                    : m_spec.disabledTextColor;

            painter->save();
            painter->setRenderHint(
                QPainter::Antialiasing,
                true);
            QPen pen(
                arrowColor,
                2.0,
                Qt::SolidLine,
                Qt::RoundCap,
                Qt::RoundJoin);
            painter->setPen(pen);
            painter->setBrush(Qt::NoBrush);
            painter->drawLine(
                QPointF(
                    center.x() - halfWidth,
                    center.y() - halfHeight),
                QPointF(
                    center.x(),
                    center.y() + halfHeight));
            painter->drawLine(
                QPointF(
                    center.x(),
                    center.y() + halfHeight),
                QPointF(
                    center.x() + halfWidth,
                    center.y() - halfHeight));
            painter->restore();
        }
    }

private:
    AutocompleteSpec m_spec;
};

class ComboBoxAdapterState final : public QObject
{
public:
    explicit ComboBoxAdapterState(QComboBox* combo)
        : QObject(combo)
        , m_combo(combo)
        , m_previousStyle(
              combo ? combo->style() : nullptr)
        , m_themeBinding(
              new QtMaterialThemeContextBinding(
                  combo,
                  this))
        , m_style(new NativeComboBoxProxyStyle(
              resolvedSpec()))
    {
        setObjectName(
            QString::fromLatin1(kStateObjectName));
        m_style->setParent(this);

        if (m_combo) {
            m_combo->installEventFilter(this);
            m_combo->setStyle(m_style);
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
        if (!m_combo) {
            return;
        }

        m_combo->removeEventFilter(this);
        if (m_combo->style() == m_style) {
            QStyle* restoreStyle =
                m_previousStyle.data();
            if (!restoreStyle) {
                restoreStyle =
                    QApplication::style();
            }
            m_combo->setStyle(restoreStyle);
        }
    }

protected:
    bool eventFilter(
        QObject* watched,
        QEvent* event) override
    {
        if (watched == m_combo
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
    AutocompleteSpec resolvedSpec() const
    {
        Q_ASSERT(m_combo);
        Q_ASSERT(m_themeBinding);
        return InputSpecResolution::autocompleteSpec(
            m_themeBinding,
            QtMaterialComboBoxAdapter::density(
                m_combo));
    }

    void refreshResolvedSpec()
    {
        if (!m_combo
            || !m_style
            || !m_themeBinding) {
            return;
        }

        m_style->setResolvedSpec(
            resolvedSpec());
        m_combo->updateGeometry();
        m_combo->update();
    }

    QPointer<QComboBox> m_combo;
    QPointer<QStyle> m_previousStyle;
    QtMaterialThemeContextBinding*
        m_themeBinding = nullptr;
    NativeComboBoxProxyStyle* m_style = nullptr;
};

ComboBoxAdapterState* adapterState(
    QComboBox* combo)
{
    if (!combo) {
        return nullptr;
    }

    QObject* object =
        combo->findChild<QObject*>(
            QString::fromLatin1(
                kStateObjectName),
            Qt::FindDirectChildrenOnly);
    return static_cast<ComboBoxAdapterState*>(
        object);
}

} // namespace

void QtMaterialComboBoxAdapter::apply(
    QComboBox* comboBox,
    Density densityValue)
{
    if (!comboBox) {
        return;
    }

    comboBox->setProperty(
        kAppliedProperty,
        true);
    comboBox->setProperty(
        kDensityProperty,
        densityName(densityValue));

    if (!adapterState(comboBox)) {
        new ComboBoxAdapterState(comboBox);
    }

    comboBox->updateGeometry();
    comboBox->update();
}

void QtMaterialComboBoxAdapter::remove(
    QComboBox* comboBox)
{
    if (!comboBox) {
        return;
    }

    if (ComboBoxAdapterState* state =
            adapterState(comboBox)) {
        state->restore();
        delete state;
    }

    comboBox->setProperty(
        kAppliedProperty,
        QVariant());
    comboBox->setProperty(
        kDensityProperty,
        QVariant());
    comboBox->updateGeometry();
    comboBox->update();
}

bool QtMaterialComboBoxAdapter::isApplied(
    const QComboBox* comboBox)
{
    return comboBox
        && comboBox->property(
                        kAppliedProperty)
               .toBool();
}

void QtMaterialComboBoxAdapter::setDensity(
    QComboBox* comboBox,
    Density densityValue)
{
    if (!comboBox) {
        return;
    }

    if (!isApplied(comboBox)) {
        apply(comboBox, densityValue);
        return;
    }

    comboBox->setProperty(
        kDensityProperty,
        densityName(densityValue));
}

Density QtMaterialComboBoxAdapter::density(
    const QComboBox* comboBox)
{
    if (!comboBox) {
        return Density::Default;
    }
    return densityFromProperty(
        comboBox->property(kDensityProperty));
}

void QtMaterialComboBoxAdapter::setOptOut(
    QComboBox* comboBox,
    bool excluded)
{
    if (!comboBox) {
        return;
    }

    if (excluded) {
        remove(comboBox);
    }
    comboBox->setProperty(
        kOptOutProperty,
        excluded);
}

bool QtMaterialComboBoxAdapter::isOptedOut(
    const QComboBox* comboBox)
{
    return comboBox
        && comboBox->property(
                        kOptOutProperty)
               .toBool();
}

int QtMaterialComboBoxAdapter::applyToDescendants(
    QWidget* root,
    Density densityValue)
{
    if (!root) {
        return 0;
    }

    int count = 0;
    if (auto* rootCombo =
            qobject_cast<QComboBox*>(root)) {
        if (!isOptedOut(rootCombo)) {
            apply(rootCombo, densityValue);
            ++count;
        }
    }

    const auto combos =
        root->findChildren<QComboBox*>();
    for (QComboBox* combo : combos) {
        if (!combo
            || isOptedOut(combo)) {
            continue;
        }
        apply(combo, densityValue);
        ++count;
    }

    return count;
}

const char*
QtMaterialComboBoxAdapter::
    appliedPropertyName() noexcept
{
    return kAppliedProperty;
}

const char*
QtMaterialComboBoxAdapter::
    densityPropertyName() noexcept
{
    return kDensityProperty;
}

const char*
QtMaterialComboBoxAdapter::
    optOutPropertyName() noexcept
{
    return kOptOutProperty;
}

} // namespace QtMaterial
