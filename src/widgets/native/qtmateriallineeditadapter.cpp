#include "qtmaterial/widgets/native/qtmateriallineeditadapter.h"

#include <QApplication>
#include <QDynamicPropertyChangeEvent>
#include <QLineEdit>
#include <QPainter>
#include <QPainterPath>
#include <QPalette>
#include <QPointer>
#include <QProxyStyle>
#include <QStyleOption>
#include <QVariant>
#include <QWidget>
#include <QtMath>

#include "../resolution/qtmaterialinputspecresolution_p.h"
#include "qtmaterial/core/private/qtmaterialthemecontextbinding_p.h"

namespace QtMaterial {
namespace {

constexpr char kAppliedProperty[] = "qtm3MaterialLineEdit";
constexpr char kVariantProperty[] = "qtm3MaterialTextFieldVariant";
constexpr char kDensityProperty[] = "qtm3MaterialDensity";
constexpr char kOptOutProperty[] = "qtm3MaterialOptOut";
constexpr char kStateObjectName[] = "_qtm3_native_lineedit_adapter_state";

QString variantName(QtMaterialLineEditAdapter::Variant variant)
{
    switch (variant) {
    case QtMaterialLineEditAdapter::Variant::Filled:
        return QStringLiteral("filled");
    case QtMaterialLineEditAdapter::Variant::Outlined:
    default:
        return QStringLiteral("outlined");
    }
}

QtMaterialLineEditAdapter::Variant variantFromProperty(const QVariant& value)
{
    const QString name = value.toString().trimmed().toLower();
    if (name == QStringLiteral("filled")) {
        return QtMaterialLineEditAdapter::Variant::Filled;
    }
    return QtMaterialLineEditAdapter::Variant::Outlined;
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

QColor withOpacity(QColor color, qreal opacity)
{
    color.setAlphaF(
        qBound<qreal>(
            0.0,
            color.alphaF() * opacity,
            1.0));
    return color;
}

class NativeLineEditProxyStyle final : public QProxyStyle
{
public:
    NativeLineEditProxyStyle(
        QtMaterialLineEditAdapter::Variant variant,
        const TextFieldSpec& spec)
        : QProxyStyle()
        , m_variant(variant)
        , m_spec(spec)
    {
    }

    void setResolvedSpec(
        QtMaterialLineEditAdapter::Variant variant,
        const TextFieldSpec& spec)
    {
        m_variant = variant;
        m_spec = spec;
    }

    void drawPrimitive(
        PrimitiveElement element,
        const QStyleOption* option,
        QPainter* painter,
        const QWidget* widget = nullptr) const override
    {
        const auto* lineEdit =
            qobject_cast<const QLineEdit*>(widget);
        const auto* frameOption =
            qstyleoption_cast<const QStyleOptionFrame*>(option);

        if (element != PE_PanelLineEdit
            || !lineEdit
            || !frameOption
            || !QtMaterialLineEditAdapter::isApplied(lineEdit)) {
            QProxyStyle::drawPrimitive(
                element,
                option,
                painter,
                widget);
            return;
        }

        QRectF fieldRect =
            QRectF(frameOption->rect).adjusted(
                0.5,
                0.5,
                -0.5,
                -0.5);
        if (!fieldRect.isValid()) {
            return;
        }

        const bool enabled = lineEdit->isEnabled();
        const bool focused = lineEdit->hasFocus();
        const bool hovered = lineEdit->underMouse();

        const qreal radius =
            qMin<qreal>(
                qMax<qreal>(0.0, m_spec.cornerRadius),
                fieldRect.height() / 2.0);

        QPainterPath path;
        path.addRoundedRect(
            fieldRect,
            radius,
            radius);

        painter->save();
        painter->setRenderHint(
            QPainter::Antialiasing,
            true);

        if (m_variant
            == QtMaterialLineEditAdapter::Variant::Filled) {
            painter->fillPath(
                path,
                m_spec.containerColor);
        }

        if (enabled && hovered) {
            painter->fillPath(
                path,
                withOpacity(
                    m_spec.stateLayerColor,
                    m_spec.hoverStateLayerOpacity));
        }

        if (m_variant
            == QtMaterialLineEditAdapter::Variant::Outlined) {
            QColor outline =
                enabled
                    ? (focused
                           ? m_spec.focusedOutlineColor
                           : m_spec.outlineColor)
                    : m_spec.disabledOutlineColor;
            painter->setBrush(Qt::NoBrush);
            painter->setPen(
                QPen(
                    outline,
                    focused
                        ? m_spec.focusedOutlineWidth
                        : m_spec.outlineWidth));
            painter->drawPath(path);
        } else {
            const QColor indicator =
                enabled
                    ? (focused
                           ? m_spec.activeIndicatorColor
                           : m_spec.outlineColor)
                    : m_spec.disabledOutlineColor;
            const qreal width =
                focused
                    ? m_spec.focusedOutlineWidth
                    : m_spec.outlineWidth;
            painter->setPen(
                QPen(
                    indicator,
                    width,
                    Qt::SolidLine,
                    Qt::SquareCap));
            painter->drawLine(
                QPointF(
                    fieldRect.left() + radius,
                    fieldRect.bottom()),
                QPointF(
                    fieldRect.right() - radius,
                    fieldRect.bottom()));
        }

        if (enabled
            && focused
            && m_spec.focusRingWidth > 0.0) {
            painter->setBrush(Qt::NoBrush);
            painter->setPen(
                QPen(
                    m_spec.focusRingColor,
                    m_spec.focusRingWidth));
            painter->drawPath(
                path);
        }

        painter->restore();
    }

    QRect subElementRect(
        SubElement element,
        const QStyleOption* option,
        const QWidget* widget = nullptr) const override
    {
        const auto* lineEdit =
            qobject_cast<const QLineEdit*>(widget);

        if (element != SE_LineEditContents
            || !lineEdit
            || !option
            || !QtMaterialLineEditAdapter::isApplied(lineEdit)) {
            return QProxyStyle::subElementRect(
                element,
                option,
                widget);
        }

        QRect result = option->rect.adjusted(
            m_spec.horizontalPadding,
            qMax(0, m_spec.verticalPadding / 2),
            -m_spec.horizontalPadding,
            -qMax(0, m_spec.verticalPadding / 2));

        if (result.width() < 0) {
            result.setWidth(0);
        }
        if (result.height() < 0) {
            result.setHeight(0);
        }
        return result;
    }

    int pixelMetric(
        PixelMetric metric,
        const QStyleOption* option = nullptr,
        const QWidget* widget = nullptr) const override
    {
        const auto* lineEdit =
            qobject_cast<const QLineEdit*>(widget);
        if (lineEdit
            && QtMaterialLineEditAdapter::isApplied(lineEdit)
            && metric == PM_DefaultFrameWidth) {
            return qMax(
                1,
                m_spec.outlineWidth);
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

        const auto* lineEdit =
            qobject_cast<const QLineEdit*>(widget);
        if (type != CT_LineEdit
            || !lineEdit
            || !QtMaterialLineEditAdapter::isApplied(lineEdit)) {
            return size;
        }

        size.setHeight(
            qMax(
                size.height(),
                m_spec.minHeight));
        return size;
    }

private:
    QtMaterialLineEditAdapter::Variant m_variant;
    TextFieldSpec m_spec;
};

class LineEditAdapterState final : public QObject
{
public:
    explicit LineEditAdapterState(QLineEdit* lineEdit)
        : QObject(lineEdit)
        , m_lineEdit(lineEdit)
        , m_previousStyle(
              lineEdit ? lineEdit->style() : nullptr)
        , m_previousPalette(
              lineEdit ? lineEdit->palette() : QPalette())
        , m_previousFont(
              lineEdit ? lineEdit->font() : QFont())
        , m_hadExplicitPalette(
              lineEdit && lineEdit->testAttribute(Qt::WA_SetPalette))
        , m_hadExplicitFont(
              lineEdit && lineEdit->testAttribute(Qt::WA_SetFont))
        , m_themeBinding(
              new QtMaterialThemeContextBinding(
                  lineEdit,
                  this))
        , m_style(
              new NativeLineEditProxyStyle(
                  currentVariant(),
                  resolvedSpec()))
    {
        setObjectName(
            QString::fromLatin1(kStateObjectName));
        m_style->setParent(this);

        if (m_lineEdit) {
            m_lineEdit->installEventFilter(this);
            m_lineEdit->setStyle(m_style);
            applyPalette();
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
        if (!m_lineEdit) {
            return;
        }

        m_lineEdit->removeEventFilter(this);

        if (m_lineEdit->style() == m_style) {
            QStyle* restoreStyle =
                m_previousStyle.data();
            if (!restoreStyle) {
                restoreStyle =
                    QApplication::style();
            }
            m_lineEdit->setStyle(restoreStyle);
        }

        // QStyle::polish() may change the effective palette/font on macOS.
        // Restore both the values and their ownership: an inherited
        // palette/font must remain inherited rather than becoming explicit
        // widget state after adaptation.
        if (m_hadExplicitPalette) {
            m_lineEdit->setPalette(m_previousPalette);
        } else {
            m_lineEdit->setPalette(QPalette());
        }

        if (m_hadExplicitFont) {
            m_lineEdit->setFont(m_previousFont);
        } else {
            m_lineEdit->setFont(QFont());
        }
    }

protected:
    bool eventFilter(
        QObject* watched,
        QEvent* event) override
    {
        if (watched == m_lineEdit
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
    QtMaterialLineEditAdapter::Variant currentVariant() const
    {
        if (!m_lineEdit) {
            return QtMaterialLineEditAdapter::Variant::Outlined;
        }
        return variantFromProperty(
            m_lineEdit->property(kVariantProperty));
    }

    TextFieldSpec resolvedSpec() const
    {
        Q_ASSERT(m_lineEdit);
        Q_ASSERT(m_themeBinding);

        const Density density =
            QtMaterialLineEditAdapter::density(
                m_lineEdit);

        if (currentVariant()
            == QtMaterialLineEditAdapter::Variant::Filled) {
            return InputSpecResolution::filledTextFieldSpec(
                m_themeBinding,
                density);
        }
        return InputSpecResolution::outlinedTextFieldSpec(
            m_themeBinding,
            density);
    }

    void applyPalette()
    {
        if (!m_lineEdit || !m_themeBinding) {
            return;
        }

        const TextFieldSpec spec = resolvedSpec();
        QPalette palette = m_previousPalette;

        palette.setColor(
            QPalette::Text,
            spec.inputTextColor);
        palette.setColor(
            QPalette::Disabled,
            QPalette::Text,
            spec.disabledInputTextColor);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setColor(
            QPalette::PlaceholderText,
            spec.labelColor);
        palette.setColor(
            QPalette::Disabled,
            QPalette::PlaceholderText,
            spec.disabledLabelColor);
#endif
        palette.setColor(
            QPalette::Base,
            Qt::transparent);

        m_lineEdit->setPalette(palette);
        if (spec.hasResolvedInputFont) {
            m_lineEdit->setFont(spec.inputFont);
        }
    }

    void refreshResolvedSpec()
    {
        if (!m_lineEdit
            || !m_style
            || !m_themeBinding) {
            return;
        }

        m_style->setResolvedSpec(
            currentVariant(),
            resolvedSpec());
        applyPalette();
        m_lineEdit->updateGeometry();
        m_lineEdit->update();
    }

    QPointer<QLineEdit> m_lineEdit;
    QPointer<QStyle> m_previousStyle;
    QPalette m_previousPalette;
    QFont m_previousFont;
    bool m_hadExplicitPalette = false;
    bool m_hadExplicitFont = false;
    QtMaterialThemeContextBinding*
        m_themeBinding = nullptr;
    NativeLineEditProxyStyle* m_style = nullptr;
};

LineEditAdapterState* adapterState(QLineEdit* lineEdit)
{
    if (!lineEdit) {
        return nullptr;
    }

    QObject* object =
        lineEdit->findChild<QObject*>(
            QString::fromLatin1(
                kStateObjectName),
            Qt::FindDirectChildrenOnly);
    return static_cast<LineEditAdapterState*>(
        object);
}

} // namespace

void QtMaterialLineEditAdapter::apply(
    QLineEdit* lineEdit,
    Variant variantValue,
    Density densityValue)
{
    if (!lineEdit) {
        return;
    }

    lineEdit->setProperty(
        kAppliedProperty,
        true);
    lineEdit->setProperty(
        kVariantProperty,
        variantName(variantValue));
    lineEdit->setProperty(
        kDensityProperty,
        densityName(densityValue));

    if (!adapterState(lineEdit)) {
        new LineEditAdapterState(lineEdit);
    }

    lineEdit->updateGeometry();
    lineEdit->update();
}

void QtMaterialLineEditAdapter::remove(
    QLineEdit* lineEdit)
{
    if (!lineEdit) {
        return;
    }

    if (LineEditAdapterState* state =
            adapterState(lineEdit)) {
        state->restore();
        delete state;
    }

    lineEdit->setProperty(
        kAppliedProperty,
        QVariant());
    lineEdit->setProperty(
        kVariantProperty,
        QVariant());
    lineEdit->setProperty(
        kDensityProperty,
        QVariant());
    lineEdit->updateGeometry();
    lineEdit->update();
}

bool QtMaterialLineEditAdapter::isApplied(
    const QLineEdit* lineEdit)
{
    return lineEdit
        && lineEdit->property(
                        kAppliedProperty)
               .toBool();
}

void QtMaterialLineEditAdapter::setVariant(
    QLineEdit* lineEdit,
    Variant variantValue)
{
    if (!lineEdit) {
        return;
    }

    if (!isApplied(lineEdit)) {
        apply(
            lineEdit,
            variantValue,
            Density::Default);
        return;
    }

    lineEdit->setProperty(
        kVariantProperty,
        variantName(variantValue));
}

QtMaterialLineEditAdapter::Variant
QtMaterialLineEditAdapter::variant(
    const QLineEdit* lineEdit)
{
    if (!lineEdit) {
        return Variant::Outlined;
    }
    return variantFromProperty(
        lineEdit->property(kVariantProperty));
}

void QtMaterialLineEditAdapter::setDensity(
    QLineEdit* lineEdit,
    Density densityValue)
{
    if (!lineEdit) {
        return;
    }

    if (!isApplied(lineEdit)) {
        apply(
            lineEdit,
            Variant::Outlined,
            densityValue);
        return;
    }

    lineEdit->setProperty(
        kDensityProperty,
        densityName(densityValue));
}

Density QtMaterialLineEditAdapter::density(
    const QLineEdit* lineEdit)
{
    if (!lineEdit) {
        return Density::Default;
    }
    return densityFromProperty(
        lineEdit->property(kDensityProperty));
}

void QtMaterialLineEditAdapter::setOptOut(
    QLineEdit* lineEdit,
    bool excluded)
{
    if (!lineEdit) {
        return;
    }

    if (excluded) {
        remove(lineEdit);
    }
    lineEdit->setProperty(
        kOptOutProperty,
        excluded);
}

bool QtMaterialLineEditAdapter::isOptedOut(
    const QLineEdit* lineEdit)
{
    return lineEdit
        && lineEdit->property(
                        kOptOutProperty)
               .toBool();
}

int QtMaterialLineEditAdapter::applyToDescendants(
    QWidget* root,
    Variant variantValue,
    Density densityValue)
{
    if (!root) {
        return 0;
    }

    int count = 0;
    if (auto* rootLineEdit =
            qobject_cast<QLineEdit*>(root)) {
        if (!isOptedOut(rootLineEdit)) {
            apply(
                rootLineEdit,
                variantValue,
                densityValue);
            ++count;
        }
    }

    const auto lineEdits =
        root->findChildren<QLineEdit*>();
    for (QLineEdit* lineEdit : lineEdits) {
        if (!lineEdit
            || isOptedOut(lineEdit)) {
            continue;
        }
        apply(
            lineEdit,
            variantValue,
            densityValue);
        ++count;
    }

    return count;
}

const char*
QtMaterialLineEditAdapter::
    appliedPropertyName() noexcept
{
    return kAppliedProperty;
}

const char*
QtMaterialLineEditAdapter::
    variantPropertyName() noexcept
{
    return kVariantProperty;
}

const char*
QtMaterialLineEditAdapter::
    densityPropertyName() noexcept
{
    return kDensityProperty;
}

const char*
QtMaterialLineEditAdapter::
    optOutPropertyName() noexcept
{
    return kOptOutProperty;
}

} // namespace QtMaterial
