#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"

#include <algorithm>
#include <QEvent>
#include <QLineEdit>
#include <QPainter>
#include <QPalette>
#include <QResizeEvent>

#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "private/qtmaterialtextfieldshellhelper_p.h"
#include "../../core/private/qtmaterialaccessibilityhelper_p.h"

namespace QtMaterial {

QtMaterialOutlinedTextField::QtMaterialOutlinedTextField(QWidget* parent)
    : QtMaterialInputControl(parent)
    , m_lineEdit(new QLineEdit(this))
    , m_transition(new QtMaterialTransitionController(this))
{
    setMinimumHeight(64);
    if (m_lineEdit) {
        m_lineEdit->setFrame(false);
        m_lineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);
        m_lineEdit->setClearButtonEnabled(false);
        m_lineEdit->installEventFilter(this);
        QObject::connect(m_lineEdit, &QLineEdit::textChanged, this, [this]() {
            syncAccessibilityState();
            update();
        });
    }

    setFocusProxy(m_lineEdit);
    syncAccessibilityState();
    syncLineEditPalette();
}

QtMaterialOutlinedTextField::~QtMaterialOutlinedTextField() = default;

QString QtMaterialOutlinedTextField::text() const
{
    return m_lineEdit ? m_lineEdit->text() : QString();
}

void QtMaterialOutlinedTextField::setText(const QString& text)
{
    if (m_lineEdit) {
        m_lineEdit->setText(text);
        syncAccessibilityState();
        update();
    }
}

QLineEdit* QtMaterialOutlinedTextField::lineEdit() const
{
    return m_lineEdit;
}

QSize QtMaterialOutlinedTextField::sizeHint() const
{
    ensureSpecResolved();
    const QFontMetrics metrics(font());
    const int textWidth = std::max({
        metrics.horizontalAdvance(labelText()),
        metrics.horizontalAdvance(supportingText()),
        metrics.horizontalAdvance(errorText()),
        120
    });
    const int width = textWidth + (2 * spec().horizontalPadding) + 24;
    const int height = spec().topLabelHeight + spec().minHeight + spec().supportingTopSpacing + spec().supportingHeight + 2;
    return QSize(width, height);
}

QSize QtMaterialOutlinedTextField::minimumSizeHint() const
{
    ensureSpecResolved();
    const int height = spec().topLabelHeight + spec().minHeight + spec().supportingTopSpacing + spec().supportingHeight + 2;
    return QSize(120, height);
}

QtMaterialOutlinedTextField::ShellVariant QtMaterialOutlinedTextField::shellVariant() const
{
    return ShellVariant::Outlined;
}

void QtMaterialOutlinedTextField::syncAccessibilityState()
{
    QtMaterialInputControl::syncAccessibilityState();
    AccessibilityHelper::applyInputAccessibility(this, m_lineEdit, labelText(), supportingText(), errorText(), hasErrorState());
}

TextFieldSpec QtMaterialOutlinedTextField::resolveTextFieldSpec(const SpecFactory& factory) const
{
    return factory.outlinedTextFieldSpec(theme(), density());
}

void QtMaterialOutlinedTextField::themeChangedEvent(const Theme& theme)
{
    QtMaterialInputControl::themeChangedEvent(theme);
    m_specDirty = true;
    invalidateLayoutCache();
    syncLineEditPalette();
}

void QtMaterialOutlinedTextField::invalidateResolvedSpec()
{
    m_specDirty = true;
    invalidateLayoutCache();
}

void QtMaterialOutlinedTextField::ensureSpecResolved() const
{
    if (!m_specDirty) {
        return;
    }

    SpecFactory factory;
    m_spec = resolveTextFieldSpec(factory);
    m_specDirty = false;
}

const TextFieldSpec& QtMaterialOutlinedTextField::spec() const
{
    ensureSpecResolved();
    return m_spec;
}

void QtMaterialOutlinedTextField::invalidateLayoutCache()
{
    m_layoutDirty = true;
}

bool QtMaterialOutlinedTextField::currentFocusState() const
{
    return (m_lineEdit && m_lineEdit->hasFocus()) || hasFocus();
}

void QtMaterialOutlinedTextField::ensureLayoutResolved() const
{
    ensureSpecResolved();
    if (!m_layoutDirty) {
        return;
    }

    const auto variant = shellVariant() == ShellVariant::Filled
        ? QtMaterialTextFieldShellHelper::Variant::Filled
        : QtMaterialTextFieldShellHelper::Variant::Outlined;
    const auto layout = QtMaterialTextFieldShellHelper::layoutFor(rect(), spec(), theme(), variant);
    const auto text = QtMaterialTextFieldShellHelper::elidedTextFor(layout, spec(), labelText(), supportingText(), errorText(), font());

    m_cachedContainerRect = layout.containerRect;
    m_cachedLabelRect = layout.labelRect;
    m_cachedEditorRect = layout.editorRect;
    m_cachedSupportingRect = layout.supportingRect;
    m_cachedFocusRect = layout.focusRect;
    m_cachedRadius = layout.radius;
    m_cachedLabelText = text.labelText;
    m_cachedSupportingText = text.supportingText;
    m_cachedErrorText = text.errorText;
    m_cachedDisplaySupportingText = hasErrorState() ? text.errorText : text.supportingText;

    m_cachedContainerPath = QPainterPath();
    m_cachedContainerPath.addRoundedRect(m_cachedContainerRect, m_cachedRadius, m_cachedRadius);

    m_layoutDirty = false;
}

void QtMaterialOutlinedTextField::syncLineEditGeometry()
{
    ensureLayoutResolved();
    if (m_lineEdit && m_lineEdit->geometry() != m_cachedEditorRect) {
        m_lineEdit->setGeometry(m_cachedEditorRect);
    }
}

void QtMaterialOutlinedTextField::syncLineEditPalette()
{
    if (!m_lineEdit) {
        return;
    }

    ensureSpecResolved();
    QPalette palette = m_lineEdit->palette();
    palette.setColor(QPalette::Text, isEnabled() ? spec().inputTextColor : spec().disabledInputTextColor);
    palette.setColor(QPalette::PlaceholderText, isEnabled() ? spec().labelColor : spec().disabledLabelColor);
    palette.setColor(QPalette::Base, Qt::transparent);
    palette.setColor(QPalette::Window, Qt::transparent);
    m_lineEdit->setPalette(palette);
}

void QtMaterialOutlinedTextField::resizeEvent(QResizeEvent* event)
{
    QtMaterialInputControl::resizeEvent(event);
    invalidateLayoutCache();
    syncLineEditGeometry();
    syncAccessibilityState();
}

void QtMaterialOutlinedTextField::changeEvent(QEvent* event)
{
    QtMaterialInputControl::changeEvent(event);

    switch (event->type()) {
    case QEvent::EnabledChange:
    case QEvent::FontChange:
    case QEvent::StyleChange:
        invalidateLayoutCache();
        syncLineEditPalette();
        syncLineEditGeometry();
        break;
    default:
        break;
    }
}

bool QtMaterialOutlinedTextField::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == m_lineEdit) {
        switch (event->type()) {
        case QEvent::FocusIn:
            interactionState().setFocused(true);
            if (m_transition) {
                m_transition->startForward();
            }
            update();
            break;
        case QEvent::FocusOut:
            interactionState().setFocused(false);
            if (m_transition) {
                m_transition->startBackward();
            }
            update();
            break;
        case QEvent::EnabledChange:
        case QEvent::FontChange:
            invalidateLayoutCache();
            syncLineEditPalette();
            updateGeometry();
            update();
            break;
        default:
            break;
        }
    }

    return QtMaterialInputControl::eventFilter(watched, event);
}

void QtMaterialOutlinedTextField::paintEvent(QPaintEvent*)
{
    ensureSpecResolved();
    ensureLayoutResolved();
    syncLineEditGeometry();
    syncLineEditPalette();

    QtMaterialInteractionState state = interactionState();
    state.setFocused(currentFocusState());
    state.setEnabled(isEnabled());

    const auto variant = shellVariant() == ShellVariant::Filled
        ? QtMaterialTextFieldShellHelper::Variant::Filled
        : QtMaterialTextFieldShellHelper::Variant::Outlined;

    QtMaterialTextFieldShellHelper::Layout layout;
    layout.containerRect = m_cachedContainerRect;
    layout.labelRect = m_cachedLabelRect;
    layout.editorRect = m_cachedEditorRect;
    layout.supportingRect = m_cachedSupportingRect;
    layout.focusRect = m_cachedFocusRect;
    layout.radius = m_cachedRadius;

    QtMaterialTextFieldShellHelper::ElidedText text;
    text.labelText = m_cachedLabelText;
    text.supportingText = m_cachedSupportingText;
    text.errorText = m_cachedErrorText;

    QPainter painter(this);
    QtMaterialTextFieldShellHelper::paintShell(
        &painter,
        layout,
        theme(),
        spec(),
        state,
        variant,
        text,
        hasErrorState(),
        font());
}

} // namespace QtMaterial
