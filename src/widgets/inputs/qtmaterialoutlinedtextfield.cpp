#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"

#include <QEvent>
#include <QFontMetrics>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QPainterPath>
#include <QPalette>
#include <QResizeEvent>
#include <QStyle>
#include <QToolButton>
#include <QtGlobal>

#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "private/qtmaterialtextfieldshellhelper_p.h"
#include "../../core/private/qtmaterialaccessibilityhelper_p.h"

namespace {

constexpr int kAccessorySpacing = 8;
constexpr int kAccessoryTextPadding = 4;
constexpr int kMinimumAccessoryTextWidth = 24;

} // namespace

namespace QtMaterial {

QtMaterialOutlinedTextField::QtMaterialOutlinedTextField(QWidget* parent)
    : QtMaterialInputControl(parent)
    , m_lineEdit(new QLineEdit(this))
    , m_prefixLabel(new QLabel(this))
    , m_suffixLabel(new QLabel(this))
    , m_leadingIconLabel(new QLabel(this))
    , m_trailingIconLabel(new QLabel(this))
    , m_endActionButton(new QToolButton(this))
    , m_transition(new QtMaterialTransitionController(this))
{
    setMinimumHeight(64);

    if (m_prefixLabel) {
        m_prefixLabel->setObjectName(QStringLiteral("prefixLabel"));
        m_prefixLabel->hide();
        m_prefixLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        m_prefixLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    }

    if (m_suffixLabel) {
        m_suffixLabel->setObjectName(QStringLiteral("suffixLabel"));
        m_suffixLabel->hide();
        m_suffixLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        m_suffixLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    }

    if (m_leadingIconLabel) {
        m_leadingIconLabel->setObjectName(QStringLiteral("leadingIconLabel"));
        m_leadingIconLabel->hide();
        m_leadingIconLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        m_leadingIconLabel->setAlignment(Qt::AlignCenter);
    }

    if (m_trailingIconLabel) {
        m_trailingIconLabel->setObjectName(QStringLiteral("trailingIconLabel"));
        m_trailingIconLabel->hide();
        m_trailingIconLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        m_trailingIconLabel->setAlignment(Qt::AlignCenter);
    }

    if (m_endActionButton) {
        m_endActionButton->setObjectName(QStringLiteral("endActionButton"));
        m_endActionButton->hide();
        m_endActionButton->setAutoRaise(true);
        m_endActionButton->setFocusPolicy(Qt::NoFocus);
        m_endActionButton->setCursor(Qt::PointingHandCursor);

        QObject::connect(m_endActionButton, &QToolButton::clicked, this, [this]() {
            switch (currentEndActionMode()) {
            case EndActionMode::ClearText:
                if (m_lineEdit) {
                    m_lineEdit->clear();
                    m_lineEdit->setFocus();
                }
                break;

            case EndActionMode::TogglePasswordVisibility:
                m_passwordVisible = !m_passwordVisible;
                if (m_lineEdit) {
                    m_lineEdit->setEchoMode(
                        m_passwordVisible ? QLineEdit::Normal : m_configuredEchoMode);
                    m_lineEdit->setFocus();
                }
                break;

            case EndActionMode::None:
            default:
                break;
            }

            invalidateLayoutCache();
            syncLineEditGeometry();
            syncAccessibilityState();
            updateGeometry();
            update();
        });
    }

    if (m_lineEdit) {
        m_lineEdit->setFrame(false);
        m_lineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);
        m_lineEdit->setClearButtonEnabled(false);
        m_lineEdit->setEchoMode(m_configuredEchoMode);
        m_lineEdit->installEventFilter(this);

        QObject::connect(m_lineEdit, &QLineEdit::textChanged, this, [this]() {
            if (m_validationFeedbackMode == ValidationFeedbackMode::ValidatorOnEdit
                || (m_validationFeedbackMode == ValidationFeedbackMode::ValidatorOnCommit
                    && m_validationCommitted)) {
                refreshValidationState(false);
            }

            syncAccessibilityState();
            invalidateLayoutCache();
            syncLineEditGeometry();
            updateGeometry();
            update();
        });

        QObject::connect(m_lineEdit, &QLineEdit::editingFinished, this, [this]() {
            refreshValidationState(true);
            syncAccessibilityState();
            invalidateLayoutCache();
            syncLineEditGeometry();
            updateGeometry();
            update();
        });
    }

    setFocusProxy(m_lineEdit);
    syncAccessibilityState();
    syncLineEditPalette();
    syncAccessoryWidgets();
}

QtMaterialOutlinedTextField::~QtMaterialOutlinedTextField() = default;

QString QtMaterialOutlinedTextField::text() const
{
    return m_lineEdit ? m_lineEdit->text() : QString();
}

void QtMaterialOutlinedTextField::setText(const QString& text)
{
    if (!m_lineEdit || m_lineEdit->text() == text) {
        return;
    }

    m_lineEdit->setText(text);

    if (m_validationFeedbackMode == ValidationFeedbackMode::ValidatorOnEdit
        || (m_validationFeedbackMode == ValidationFeedbackMode::ValidatorOnCommit
            && m_validationCommitted)) {
        refreshValidationState(false);
    }

    syncAccessibilityState();
    invalidateLayoutCache();
    syncLineEditGeometry();
    updateGeometry();
    update();
}

QString QtMaterialOutlinedTextField::placeholderText() const
{
    return m_lineEdit ? m_lineEdit->placeholderText() : QString();
}

void QtMaterialOutlinedTextField::setPlaceholderText(const QString& text)
{
    if (!m_lineEdit || m_lineEdit->placeholderText() == text) {
        return;
    }

    m_lineEdit->setPlaceholderText(text);
    syncAccessibilityState();
    update();
}

QString QtMaterialOutlinedTextField::prefixText() const
{
    return m_prefixText;
}

void QtMaterialOutlinedTextField::setPrefixText(const QString& text)
{
    if (m_prefixText == text) {
        return;
    }

    m_prefixText = text;
    invalidateLayoutCache();
    syncLineEditGeometry();
    updateGeometry();
    update();
}

QString QtMaterialOutlinedTextField::suffixText() const
{
    return m_suffixText;
}

void QtMaterialOutlinedTextField::setSuffixText(const QString& text)
{
    if (m_suffixText == text) {
        return;
    }

    m_suffixText = text;
    invalidateLayoutCache();
    syncLineEditGeometry();
    updateGeometry();
    update();
}

QIcon QtMaterialOutlinedTextField::leadingIcon() const
{
    return m_leadingIcon;
}

void QtMaterialOutlinedTextField::setLeadingIcon(const QIcon& icon)
{
    if (m_leadingIcon.cacheKey() == icon.cacheKey()) {
        return;
    }

    m_leadingIcon = icon;
    invalidateLayoutCache();
    syncLineEditGeometry();
    updateGeometry();
    update();
}

QIcon QtMaterialOutlinedTextField::trailingIcon() const
{
    return m_trailingIcon;
}

void QtMaterialOutlinedTextField::setTrailingIcon(const QIcon& icon)
{
    if (m_trailingIcon.cacheKey() == icon.cacheKey()) {
        return;
    }

    m_trailingIcon = icon;
    invalidateLayoutCache();
    syncLineEditGeometry();
    updateGeometry();
    update();
}

bool QtMaterialOutlinedTextField::isClearButtonEnabled() const noexcept
{
    return m_clearButtonEnabled;
}

void QtMaterialOutlinedTextField::setClearButtonEnabled(bool enabled)
{
    if (m_clearButtonEnabled == enabled) {
        return;
    }

    m_clearButtonEnabled = enabled;
    invalidateLayoutCache();
    syncLineEditGeometry();
    updateGeometry();
    update();
}

QLineEdit::EchoMode QtMaterialOutlinedTextField::echoMode() const
{
    return m_configuredEchoMode;
}

void QtMaterialOutlinedTextField::setEchoMode(QLineEdit::EchoMode mode)
{
    if (m_configuredEchoMode == mode) {
        if (mode != QLineEdit::Password && m_passwordVisible) {
            m_passwordVisible = false;
        }
    } else {
        m_configuredEchoMode = mode;
        if (m_configuredEchoMode != QLineEdit::Password) {
            m_passwordVisible = false;
        }
    }

    if (m_lineEdit) {
        m_lineEdit->setEchoMode(
            (m_configuredEchoMode == QLineEdit::Password && m_passwordVisible)
                ? QLineEdit::Normal
                : m_configuredEchoMode);
    }

    invalidateLayoutCache();
    syncLineEditGeometry();
    updateGeometry();
    update();
}

QLineEdit* QtMaterialOutlinedTextField::lineEdit() const
{
    return m_lineEdit;
}

const QValidator* QtMaterialOutlinedTextField::validator() const
{
    return m_lineEdit ? m_lineEdit->validator() : nullptr;
}

void QtMaterialOutlinedTextField::setValidator(const QValidator* validator)
{
    if (!m_lineEdit || m_lineEdit->validator() == validator) {
        return;
    }

    m_lineEdit->setValidator(validator);
    refreshValidationState(false);
    syncAccessibilityState();
    update();
}

QString QtMaterialOutlinedTextField::inputMask() const
{
    return m_lineEdit ? m_lineEdit->inputMask() : QString();
}

void QtMaterialOutlinedTextField::setInputMask(const QString& inputMask)
{
    if (!m_lineEdit || m_lineEdit->inputMask() == inputMask) {
        return;
    }

    m_lineEdit->setInputMask(inputMask);
    refreshValidationState(false);
    syncAccessibilityState();
    update();
}

QtMaterialOutlinedTextField::ValidationFeedbackMode
QtMaterialOutlinedTextField::validationFeedbackMode() const noexcept
{
    return m_validationFeedbackMode;
}

void QtMaterialOutlinedTextField::setValidationFeedbackMode(ValidationFeedbackMode mode)
{
    if (m_validationFeedbackMode == mode) {
        return;
    }

    m_validationFeedbackMode = mode;

    if (m_validationFeedbackMode == ValidationFeedbackMode::ManualOnly) {
        m_validationCommitted = false;
        m_automaticValidationError = false;
        syncEffectiveErrorState();
    } else if (m_validationFeedbackMode == ValidationFeedbackMode::ValidatorOnCommit) {
        m_validationCommitted = false;
        m_automaticValidationError = false;
        syncEffectiveErrorState();
    } else {
        refreshValidationState(false);
    }

    syncAccessibilityState();
    update();
}

bool QtMaterialOutlinedTextField::hasAutomaticValidationError() const noexcept
{
    return m_automaticValidationError;
}

bool QtMaterialOutlinedTextField::hasErrorState() const noexcept
{
    return m_manualErrorState || m_automaticValidationError;
}

void QtMaterialOutlinedTextField::setHasErrorState(bool value)
{
    if (m_manualErrorState == value) {
        return;
    }

    m_manualErrorState = value;
    syncEffectiveErrorState();
}

QSize QtMaterialOutlinedTextField::sizeHint() const
{
    ensureSpecResolved();

    const int width = qMax(120, 120 + effectiveLeadingReserve() + effectiveTrailingReserve());
    const int height =
        spec().topLabelHeight + spec().minHeight + spec().supportingTopSpacing
        + spec().supportingHeight + 2;

    return QSize(width, height);
}

QSize QtMaterialOutlinedTextField::minimumSizeHint() const
{
    ensureSpecResolved();

    const int width = qMax(120, 120 + effectiveLeadingReserve() + effectiveTrailingReserve());
    const int height =
        spec().topLabelHeight + spec().minHeight + spec().supportingTopSpacing
        + spec().supportingHeight + 2;

    return QSize(width, height);
}

QtMaterialOutlinedTextField::ShellVariant QtMaterialOutlinedTextField::shellVariant() const
{
    return ShellVariant::Outlined;
}

void QtMaterialOutlinedTextField::themeChangedEvent(const QtMaterial::Theme& theme)
{
    QtMaterialInputControl::themeChangedEvent(theme);
    m_specDirty = true;
    invalidateLayoutCache();
    syncLineEditPalette();
    syncAccessoryWidgets();
}

void QtMaterialOutlinedTextField::invalidateResolvedSpec()
{
    m_specDirty = true;
    invalidateLayoutCache();
}

void QtMaterialOutlinedTextField::contentChangedEvent()
{
    invalidateLayoutCache();
    syncLineEditGeometry();
}

void QtMaterialOutlinedTextField::syncAccessibilityState()
{
    QtMaterialInputControl::syncAccessibilityState();
    AccessibilityHelper::applyInputAccessibility(
        this,
        m_lineEdit,
        labelText(),
        supportingText(),
        errorText(),
        hasErrorState());
}

TextFieldSpec QtMaterialOutlinedTextField::resolveTextFieldSpec(const SpecFactory& factory) const
{
    return factory.outlinedTextFieldSpec(theme(), density());
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

QtMaterialOutlinedTextField::EndActionMode
QtMaterialOutlinedTextField::currentEndActionMode() const noexcept
{
    if (m_configuredEchoMode == QLineEdit::Password) {
        return EndActionMode::TogglePasswordVisibility;
    }

    if (m_clearButtonEnabled && m_lineEdit && !m_lineEdit->text().isEmpty() && isEnabled()) {
        return EndActionMode::ClearText;
    }

    return EndActionMode::None;
}

int QtMaterialOutlinedTextField::iconExtent() const
{
    ensureSpecResolved();
    return qMax(18, qMin(20, spec().minHeight - (2 * spec().verticalPadding)));
}

int QtMaterialOutlinedTextField::effectiveLeadingReserve() const
{
    const QFontMetrics fm(font());
    int width = 0;
    int visibleCount = 0;

    if (!m_leadingIcon.isNull()) {
        width += iconExtent();
        ++visibleCount;
    }

    if (!m_prefixText.isEmpty()) {
        const int prefixWidth =
            qMax(kMinimumAccessoryTextWidth,
                 fm.horizontalAdvance(m_prefixText) + (2 * kAccessoryTextPadding));
        width += prefixWidth;
        ++visibleCount;
    }

    if (visibleCount > 1) {
        width += (visibleCount - 1) * kAccessorySpacing;
    }

    if (visibleCount > 0) {
        width += kAccessorySpacing;
    }

    return width;
}

int QtMaterialOutlinedTextField::effectiveTrailingReserve() const
{
    const QFontMetrics fm(font());
    int width = 0;
    int visibleCount = 0;

    if (!m_suffixText.isEmpty()) {
        const int suffixWidth =
            qMax(kMinimumAccessoryTextWidth,
                 fm.horizontalAdvance(m_suffixText) + (2 * kAccessoryTextPadding));
        width += suffixWidth;
        ++visibleCount;
    }

    int endSlotWidth = 0;

    if (currentEndActionMode() != EndActionMode::None) {
        if (currentEndActionMode() == EndActionMode::ClearText) {
            endSlotWidth = iconExtent() + 6;
        } else {
            endSlotWidth = fm.horizontalAdvance(m_passwordVisible ? tr("Hide") : tr("Show")) + 16;
        }
    } else if (!m_trailingIcon.isNull()) {
        endSlotWidth = iconExtent();
    }

    if (endSlotWidth > 0) {
        width += endSlotWidth;
        ++visibleCount;
    }

    if (visibleCount > 1) {
        width += (visibleCount - 1) * kAccessorySpacing;
    }

    if (visibleCount > 0) {
        width += kAccessorySpacing;
    }

    return width;
}

bool QtMaterialOutlinedTextField::currentValidationError() const
{
    if (!m_lineEdit || !isEnabled()) {
        return false;
    }

    if (m_lineEdit->text().isEmpty()) {
        return false;
    }

    return !m_lineEdit->hasAcceptableInput();
}

void QtMaterialOutlinedTextField::refreshValidationState(bool commit)
{
    switch (m_validationFeedbackMode) {
    case ValidationFeedbackMode::ManualOnly:
        m_validationCommitted = false;
        m_automaticValidationError = false;
        break;

    case ValidationFeedbackMode::ValidatorOnEdit:
        m_validationCommitted = true;
        m_automaticValidationError = currentValidationError();
        break;

    case ValidationFeedbackMode::ValidatorOnCommit:
        if (commit) {
            m_validationCommitted = true;
        }

        if (m_validationCommitted) {
            m_automaticValidationError = currentValidationError();
        } else {
            m_automaticValidationError = false;
        }
        break;
    }

    syncEffectiveErrorState();
}

void QtMaterialOutlinedTextField::syncEffectiveErrorState()
{
    QtMaterialInputControl::setHasErrorState(m_manualErrorState || m_automaticValidationError);
}

void QtMaterialOutlinedTextField::ensureLayoutResolved() const
{
    ensureSpecResolved();

    if (!m_layoutDirty) {
        return;
    }

    QtMaterialTextFieldShellHelper::Accessories accessories;
    accessories.prefixText = m_prefixText;
    accessories.suffixText = m_suffixText;
    accessories.hasLeadingIcon = !m_leadingIcon.isNull();
    accessories.hasTrailingIcon = !m_trailingIcon.isNull();
    accessories.passwordVisible = m_passwordVisible;
    accessories.layoutDirection = layoutDirection();
    accessories.iconExtent = iconExtent();
    accessories.accessorySpacing = kAccessorySpacing;
    accessories.accessoryTextPadding = kAccessoryTextPadding;
    accessories.minimumAccessoryTextWidth = kMinimumAccessoryTextWidth;

    switch (currentEndActionMode()) {
    case EndActionMode::ClearText:
        accessories.endActionMode = QtMaterialTextFieldShellHelper::EndActionMode::ClearText;
        break;

    case EndActionMode::TogglePasswordVisibility:
        accessories.endActionMode =
            QtMaterialTextFieldShellHelper::EndActionMode::TogglePasswordVisibility;
        break;

    case EndActionMode::None:
    default:
        accessories.endActionMode = QtMaterialTextFieldShellHelper::EndActionMode::None;
        break;
    }

    const QtMaterialTextFieldShellHelper::Variant variant =
        shellVariant() == ShellVariant::Filled
            ? QtMaterialTextFieldShellHelper::Variant::Filled
            : QtMaterialTextFieldShellHelper::Variant::Outlined;

    const QtMaterialTextFieldShellHelper::Layout layout =
        QtMaterialTextFieldShellHelper::layoutFor(
            rect(), spec(), theme(), variant, accessories, font());

    const QtMaterialTextFieldShellHelper::ElidedText text =
        QtMaterialTextFieldShellHelper::elidedTextFor(
            layout,
            spec(),
            accessories,
            labelText(),
            supportingText(),
            errorText(),
            font());

    m_cachedContainerRect = layout.containerRect;
    m_cachedLabelRect = layout.labelRect;
    m_cachedEditorRect = layout.editorRect;
    m_cachedSupportingRect = layout.supportingRect;
    m_cachedFocusRect = layout.focusRect;
    m_cachedLeadingIconRect = layout.leadingIconRect;
    m_cachedPrefixRect = layout.prefixRect;
    m_cachedSuffixRect = layout.suffixRect;
    m_cachedTrailingIconRect = layout.trailingIconRect;
    m_cachedEndActionRect = layout.endActionRect;
    m_cachedRadius = layout.radius;

    m_cachedLabelText = text.labelText;
    m_cachedSupportingText = text.supportingText;
    m_cachedErrorText = text.errorText;
    m_cachedPrefixText = text.prefixText;
    m_cachedSuffixText = text.suffixText;
    m_cachedEndActionText = text.endActionText;
    m_cachedDisplaySupportingText = hasErrorState() ? text.errorText : text.supportingText;

    m_cachedLabelFont = font();
    m_cachedSupportingFont = font();

    m_cachedContainerPath = QPainterPath();
    m_cachedContainerPath.addRoundedRect(
        m_cachedContainerRect,
        m_cachedRadius,
        m_cachedRadius);

    m_layoutDirty = false;
}

void QtMaterialOutlinedTextField::syncLineEditGeometry()
{
    ensureLayoutResolved();

    if (m_lineEdit && m_lineEdit->geometry() != m_cachedEditorRect) {
        m_lineEdit->setGeometry(m_cachedEditorRect);
    }

    syncAccessoryWidgets();
}

void QtMaterialOutlinedTextField::syncAccessoryWidgets()
{
    ensureLayoutResolved();
    ensureSpecResolved();

    const QColor textColor =
        isEnabled() ? spec().inputTextColor : spec().disabledInputTextColor;
    const QColor secondaryTextColor =
        isEnabled() ? spec().labelColor : spec().disabledLabelColor;
    const bool rtl = (layoutDirection() == Qt::RightToLeft);

    auto applyTextPalette = [](QLabel* label, const QColor& color) {
        if (!label) {
            return;
        }
        QPalette palette = label->palette();
        palette.setColor(QPalette::WindowText, color);
        label->setPalette(palette);
    };

    applyTextPalette(m_prefixLabel, textColor);
    applyTextPalette(m_suffixLabel, textColor);

    if (m_prefixLabel) {
        m_prefixLabel->setFont(font());
        if (m_cachedPrefixRect.isEmpty() || m_cachedPrefixText.isEmpty()) {
            m_prefixLabel->hide();
        } else {
            m_prefixLabel->setGeometry(m_cachedPrefixRect);
            m_prefixLabel->setAlignment(Qt::AlignVCenter | (rtl ? Qt::AlignRight : Qt::AlignLeft));
            m_prefixLabel->setText(m_cachedPrefixText);
            m_prefixLabel->show();
        }
    }

    if (m_suffixLabel) {
        m_suffixLabel->setFont(font());
        if (m_cachedSuffixRect.isEmpty() || m_cachedSuffixText.isEmpty()) {
            m_suffixLabel->hide();
        } else {
            m_suffixLabel->setGeometry(m_cachedSuffixRect);
            m_suffixLabel->setAlignment(Qt::AlignVCenter | (rtl ? Qt::AlignRight : Qt::AlignLeft));
            m_suffixLabel->setText(m_cachedSuffixText);
            m_suffixLabel->show();
        }
    }

    const int iconSize = iconExtent();

    if (m_leadingIconLabel) {
        if (m_cachedLeadingIconRect.isEmpty() || m_leadingIcon.isNull()) {
            m_leadingIconLabel->hide();
        } else {
            m_leadingIconLabel->setPixmap(
                m_leadingIcon.pixmap(QSize(iconSize, iconSize),
                                     isEnabled() ? QIcon::Normal : QIcon::Disabled));
            m_leadingIconLabel->setGeometry(m_cachedLeadingIconRect);
            m_leadingIconLabel->show();
        }
    }

    if (m_trailingIconLabel) {
        if (m_cachedTrailingIconRect.isEmpty() || m_trailingIcon.isNull()) {
            m_trailingIconLabel->hide();
        } else {
            m_trailingIconLabel->setPixmap(
                m_trailingIcon.pixmap(QSize(iconSize, iconSize),
                                      isEnabled() ? QIcon::Normal : QIcon::Disabled));
            m_trailingIconLabel->setGeometry(m_cachedTrailingIconRect);
            m_trailingIconLabel->show();
        }
    }

    if (m_endActionButton) {
        QPalette palette = m_endActionButton->palette();
        palette.setColor(QPalette::ButtonText, secondaryTextColor);
        m_endActionButton->setPalette(palette);
        m_endActionButton->setEnabled(isEnabled());
        m_endActionButton->setFont(font());

        const EndActionMode mode = currentEndActionMode();

        if (m_cachedEndActionRect.isEmpty() || mode == EndActionMode::None) {
            m_endActionButton->hide();
        } else {
            m_endActionButton->setGeometry(m_cachedEndActionRect);
            m_endActionButton->setIcon(QIcon());
            m_endActionButton->setText(QString());
            m_endActionButton->setToolTip(QString());

            if (mode == EndActionMode::ClearText) {
                const QIcon clearIcon = style()->standardIcon(QStyle::SP_LineEditClearButton);
                if (clearIcon.isNull()) {
                    m_endActionButton->setText(QStringLiteral("×"));
                } else {
                    m_endActionButton->setIcon(clearIcon);
                    m_endActionButton->setIconSize(QSize(iconSize, iconSize));
                }
                m_endActionButton->setToolTip(tr("Clear text"));
            } else if (mode == EndActionMode::TogglePasswordVisibility) {
                m_endActionButton->setText(m_cachedEndActionText);
                m_endActionButton->setToolTip(
                    m_passwordVisible ? tr("Hide password") : tr("Show password"));
            }

            m_endActionButton->show();
        }
    }
}

void QtMaterialOutlinedTextField::syncLineEditPalette()
{
    if (!m_lineEdit) {
        return;
    }

    ensureSpecResolved();

    QPalette palette = m_lineEdit->palette();
    palette.setColor(
        QPalette::Text,
        isEnabled() ? spec().inputTextColor : spec().disabledInputTextColor);
    palette.setColor(
        QPalette::PlaceholderText,
        isEnabled() ? spec().labelColor : spec().disabledLabelColor);
    palette.setColor(QPalette::Base, Qt::transparent);
    palette.setColor(QPalette::Window, Qt::transparent);
    m_lineEdit->setPalette(palette);

    syncAccessoryWidgets();
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
    case QEvent::LayoutDirectionChange:
        invalidateLayoutCache();
        syncLineEditPalette();
        syncLineEditGeometry();
        refreshValidationState(false);
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
            if (m_validationFeedbackMode == ValidationFeedbackMode::ValidatorOnCommit) {
                refreshValidationState(true);
            }
            update();
            break;

        case QEvent::EnabledChange:
        case QEvent::FontChange:
        case QEvent::PaletteChange:
        case QEvent::StyleChange:
            invalidateLayoutCache();
            syncLineEditPalette();
            syncLineEditGeometry();
            refreshValidationState(false);
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
    state.setError(hasErrorState());

    QPainter painter(this);

    QtMaterialTextFieldShellHelper::Layout layout;
    layout.containerRect = m_cachedContainerRect;
    layout.labelRect = m_cachedLabelRect;
    layout.editorRect = m_cachedEditorRect;
    layout.supportingRect = m_cachedSupportingRect;
    layout.focusRect = m_cachedFocusRect;
    layout.leadingIconRect = m_cachedLeadingIconRect;
    layout.prefixRect = m_cachedPrefixRect;
    layout.suffixRect = m_cachedSuffixRect;
    layout.trailingIconRect = m_cachedTrailingIconRect;
    layout.endActionRect = m_cachedEndActionRect;
    layout.radius = m_cachedRadius;

    QtMaterialTextFieldShellHelper::ElidedText text;
    text.labelText = m_cachedLabelText;
    text.supportingText = m_cachedSupportingText;
    text.errorText = m_cachedErrorText;
    text.prefixText = m_cachedPrefixText;
    text.suffixText = m_cachedSuffixText;
    text.endActionText = m_cachedEndActionText;

    QtMaterialTextFieldShellHelper::paintShell(
        &painter,
        layout,
        theme(),
        spec(),
        state,
        shellVariant() == ShellVariant::Filled
            ? QtMaterialTextFieldShellHelper::Variant::Filled
            : QtMaterialTextFieldShellHelper::Variant::Outlined,
        text,
        hasErrorState(),
        m_cachedLabelFont);
}

} // namespace QtMaterial