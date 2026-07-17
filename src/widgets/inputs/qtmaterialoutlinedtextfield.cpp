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
#include "private/qtmaterialtextfieldshellhelper_p.h"
#include "../../core/private/qtmaterialaccessibilityhelper_p.h"
#include <memory>
#include <QShowEvent>
#include "qtmaterial/specs/qtmaterialtextfieldspecresolver.h"
#include <QScopedValueRollback>

namespace {

constexpr int kAccessorySpacing = 8;
constexpr int kAccessoryTextPadding = 4;
constexpr int kMinimumAccessoryTextWidth = 24;

} // namespace

namespace QtMaterial {

namespace {

QChar qtm3InputMaskBlankCharacter(const QString& mask)
{
    const int separator = mask.lastIndexOf(QLatin1Char(';'));
    if (separator >= 0 && separator + 1 < mask.size()) {
        return mask.at(separator + 1);
    }

    return QLatin1Char(' ');
}

QString qtm3InputMaskBody(const QString& mask)
{
    const int separator = mask.lastIndexOf(QLatin1Char(';'));
    return separator >= 0 ? mask.left(separator) : mask;
}

bool qtm3IsInputMaskSlot(const QChar ch)
{
    switch (ch.toLatin1()) {
    case 'A':
    case 'a':
    case 'N':
    case 'n':
    case 'X':
    case 'x':
    case '9':
    case '0':
    case 'D':
    case 'd':
    case '#':
    case 'H':
    case 'h':
    case 'B':
    case 'b':
        return true;
    default:
        return false;
    }
}

bool qtm3InputMaskHasUserInput(const QLineEdit* lineEdit)
{
    if (!lineEdit || lineEdit->inputMask().isEmpty()) {
        return false;
    }

    if (!lineEdit->text().isEmpty()) {
        return true;
    }

    const QString mask = qtm3InputMaskBody(lineEdit->inputMask());
    const QString display = lineEdit->displayText();
    const QChar blank = qtm3InputMaskBlankCharacter(lineEdit->inputMask());

    int displayIndex = 0;
    bool escaped = false;

    for (int i = 0; i < mask.size() && displayIndex < display.size(); ++i) {
        const QChar maskChar = mask.at(i);

        if (escaped) {
            ++displayIndex;
            escaped = false;
            continue;
        }

        if (maskChar == QLatin1Char('\\')) {
            escaped = true;
            continue;
        }

        if (maskChar == QLatin1Char('<')
            || maskChar == QLatin1Char('>')
            || maskChar == QLatin1Char('!')) {
            continue;
        }

        if (qtm3IsInputMaskSlot(maskChar)) {
            const QChar displayChar = display.at(displayIndex);
            if (!displayChar.isSpace() && displayChar != blank) {
                return true;
            }
        }

        ++displayIndex;
    }

    return false;
}

bool qtm3InputMaskHasBlankSlot(const QLineEdit* lineEdit)
{
    if (!lineEdit || lineEdit->inputMask().isEmpty()) {
        return false;
    }

    const QString mask = qtm3InputMaskBody(lineEdit->inputMask());
    const QString display = lineEdit->displayText();
    const QChar blank = qtm3InputMaskBlankCharacter(lineEdit->inputMask());

    int displayIndex = 0;
    bool escaped = false;

    for (int i = 0; i < mask.size() && displayIndex < display.size(); ++i) {
        const QChar maskChar = mask.at(i);

        if (escaped) {
            ++displayIndex;
            escaped = false;
            continue;
        }

        if (maskChar == QLatin1Char('\\')) {
            escaped = true;
            continue;
        }

        if (maskChar == QLatin1Char('<')
            || maskChar == QLatin1Char('>')
            || maskChar == QLatin1Char('!')) {
            continue;
        }

        if (qtm3IsInputMaskSlot(maskChar)) {
            const QChar displayChar = display.at(displayIndex);
            if (displayChar == blank || displayChar.isSpace()) {
                return true;
            }
        }

        ++displayIndex;
    }

    return false;
}

bool qtm3InputMaskIsIncomplete(const QLineEdit* lineEdit)
{
    if (!lineEdit || lineEdit->inputMask().isEmpty()) {
        return false;
    }

    if (!qtm3InputMaskHasUserInput(lineEdit)) {
        return false;
    }

    return !lineEdit->hasAcceptableInput() || qtm3InputMaskHasBlankSlot(lineEdit);
}

} // namespace


struct QtMaterialOutlinedTextFieldPrivate {

    explicit QtMaterialOutlinedTextFieldPrivate(QtMaterialOutlinedTextField* q)
        : m_lineEdit(new QLineEdit(q))
        , m_prefixLabel(new QLabel(q))
        , m_suffixLabel(new QLabel(q))
        , m_leadingIconLabel(new QLabel(q))
        , m_trailingIconLabel(new QLabel(q))
        , m_endActionButton(new QToolButton(q))
        , m_transition(new QtMaterialTransitionController(q))
    {
    }

    mutable bool m_specDirty = true;
    mutable bool m_layoutDirty = true;
    mutable TextFieldSpec m_spec;
    mutable QRect m_cachedContainerRect;
    mutable QRect m_cachedLabelRect;
    mutable QRect m_cachedEditorRect;
    mutable QRect m_cachedSupportingRect;
    mutable QRect m_cachedFocusRect;
    mutable QRect m_cachedLeadingIconRect;
    mutable QRect m_cachedPrefixRect;
    mutable QRect m_cachedSuffixRect;
    mutable QRect m_cachedTrailingIconRect;
    mutable QRect m_cachedEndActionRect;
    mutable qreal m_cachedRadius = 0.0;
    mutable QString m_cachedLabelText;
    mutable QString m_cachedSupportingText;
    mutable QString m_cachedErrorText;
    mutable QString m_cachedDisplaySupportingText;
    mutable QString m_cachedPrefixText;
    mutable QString m_cachedSuffixText;
    mutable QString m_cachedEndActionText;
    mutable QFont m_cachedLabelFont;
    mutable QFont m_cachedSupportingFont;
    mutable QPainterPath m_cachedContainerPath;
    QPointer<QLineEdit> m_lineEdit;
    QPointer<QLabel> m_prefixLabel;
    QPointer<QLabel> m_suffixLabel;
    QPointer<QLabel> m_leadingIconLabel;
    QPointer<QLabel> m_trailingIconLabel;
    QPointer<QToolButton> m_endActionButton;
    QString m_prefixText;
    QString m_suffixText;
    QIcon m_leadingIcon;
    QIcon m_trailingIcon;
    bool m_clearButtonEnabled = false;
    QLineEdit::EchoMode m_configuredEchoMode = QLineEdit::Normal;
    bool m_passwordVisible = false;
    QtMaterialOutlinedTextField::ValidationFeedbackMode m_validationFeedbackMode = QtMaterialOutlinedTextField::ValidationFeedbackMode::ManualOnly;
    bool m_manualErrorState = false;
    bool m_automaticValidationError = false;
    bool m_validationCommitted = false;
    QtMaterialOutlinedTextField::EndActionMode m_endActionMode = QtMaterialOutlinedTextField::EndActionMode::None;
    QIcon m_trailingActionIcon;
    QString m_trailingActionText;
    QString m_trailingActionToolTip;
    bool m_trailingActionVisibleWhenEmpty = false;
    QtMaterialTransitionController* m_transition = nullptr;
    mutable bool m_resolvingLayout = false;
    bool m_syncingGeometry = false;
    bool m_syncingCounter = false;
    bool m_refreshingValidation = false;
};


QtMaterialOutlinedTextField::QtMaterialOutlinedTextField(QWidget* parent)
    : QtMaterialInputControl(parent)
    , d_ptr(std::make_unique<QtMaterialOutlinedTextFieldPrivate>(this))
{
    setMinimumHeight(64);

    if (d_ptr->m_prefixLabel) {
        d_ptr->m_prefixLabel->setObjectName(QStringLiteral("prefixLabel"));
        d_ptr->m_prefixLabel->hide();
        d_ptr->m_prefixLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        d_ptr->m_prefixLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    }

    if (d_ptr->m_suffixLabel) {
        d_ptr->m_suffixLabel->setObjectName(QStringLiteral("suffixLabel"));
        d_ptr->m_suffixLabel->hide();
        d_ptr->m_suffixLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        d_ptr->m_suffixLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    }

    if (d_ptr->m_leadingIconLabel) {
        d_ptr->m_leadingIconLabel->setObjectName(QStringLiteral("leadingIconLabel"));
        d_ptr->m_leadingIconLabel->hide();
        d_ptr->m_leadingIconLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        d_ptr->m_leadingIconLabel->setAlignment(Qt::AlignCenter);
    }

    if (d_ptr->m_trailingIconLabel) {
        d_ptr->m_trailingIconLabel->setObjectName(QStringLiteral("trailingIconLabel"));
        d_ptr->m_trailingIconLabel->hide();
        d_ptr->m_trailingIconLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        d_ptr->m_trailingIconLabel->setAlignment(Qt::AlignCenter);
    }

    if (d_ptr->m_endActionButton) {
        d_ptr->m_endActionButton->setObjectName(QStringLiteral("endActionButton"));
        d_ptr->m_endActionButton->hide();
        d_ptr->m_endActionButton->setAutoRaise(true);
        d_ptr->m_endActionButton->setFocusPolicy(Qt::StrongFocus);
        d_ptr->m_endActionButton->setCursor(Qt::PointingHandCursor);

        QObject::connect(d_ptr->m_endActionButton, &QToolButton::clicked, this, [this]() {
            switch (resolvedEndActionMode()) {
            case EndActionMode::ClearText:
                if (d_ptr->m_lineEdit) {
                    d_ptr->m_lineEdit->clear();
                    d_ptr->m_lineEdit->setFocus();
                }
                emit clearTriggered();
                break;

            case EndActionMode::TogglePasswordVisibility:
                d_ptr->m_passwordVisible = !d_ptr->m_passwordVisible;
                if (d_ptr->m_lineEdit) {
                    d_ptr->m_lineEdit->setEchoMode(
                        d_ptr->m_passwordVisible ? QLineEdit::Normal : d_ptr->m_configuredEchoMode);
                    d_ptr->m_lineEdit->setFocus();
                }
                emit passwordVisibilityChanged(d_ptr->m_passwordVisible);
                break;

            case EndActionMode::CustomTrailingAction:
                if (d_ptr->m_lineEdit) {
                    d_ptr->m_lineEdit->setFocus();
                }
                emit trailingActionTriggered();
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

    if (d_ptr->m_lineEdit) {
        d_ptr->m_lineEdit->setFrame(false);

        setFocusProxy(d_ptr->m_lineEdit);
        d_ptr->m_lineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);
        d_ptr->m_lineEdit->setClearButtonEnabled(false);
        d_ptr->m_lineEdit->setEchoMode(d_ptr->m_configuredEchoMode);
        d_ptr->m_lineEdit->installEventFilter(this);

        QObject::connect(d_ptr->m_lineEdit, &QLineEdit::textChanged, this, [this]() {
            if (d_ptr->m_validationFeedbackMode == ValidationFeedbackMode::ValidatorOnEdit
                || (d_ptr->m_validationFeedbackMode == ValidationFeedbackMode::ValidatorOnCommit
                    && d_ptr->m_validationCommitted)) {
                refreshValidationState(false);
            }

            syncAccessibilityState();
            invalidateLayoutCache();
            syncLineEditGeometry();
            updateGeometry();
            update();
        });

        QObject::connect(d_ptr->m_lineEdit, &QLineEdit::textEdited, this, [this](const QString&) {
            updateModifiedStateFromLineEdit();
            updateTouchedState(true);
        });
        QObject::connect(d_ptr->m_lineEdit, &QLineEdit::editingFinished, this, [this]() {
            refreshValidationState(true);
            syncAccessibilityState();
            invalidateLayoutCache();
            syncLineEditGeometry();
            updateGeometry();
            update();
        });
    }
    QObject::connect(this, &QtMaterialOutlinedTextField::touchedChanged, this, [this](bool) {
        syncEffectiveErrorVisibility();
    });
    QObject::connect(this, &QtMaterialOutlinedTextField::modifiedChanged, this, [this](bool) {
        syncEffectiveErrorVisibility();
    });

    syncAccessoryWidgets();

    ensureCharacterCounterLabel();
}

QtMaterialOutlinedTextField::~QtMaterialOutlinedTextField()
{
    if (d_ptr && d_ptr->m_lineEdit) {
        d_ptr->m_lineEdit->removeEventFilter(this);
        d_ptr->m_lineEdit->disconnect(this);
    }
}

QString QtMaterialOutlinedTextField::text() const
{
    return d_ptr->m_lineEdit ? d_ptr->m_lineEdit->text() : QString();
}

void QtMaterialOutlinedTextField::setText(const QString& text)
{
    if (!d_ptr->m_lineEdit) {
        return;
    }

    const bool sameText = d_ptr->m_lineEdit->text() == text;
    const bool wasModified = d_ptr->m_lineEdit->isModified();

    if (sameText && !wasModified) {
        return;
    }

    d_ptr->m_lineEdit->setText(text);

    // QLineEdit::setText() resets QLineEdit::modified to false.
    // Keep the wrapper's cached modified state and signal contract in sync.
    updateModifiedStateFromLineEdit();

    syncCharacterCounterWidget();

    if (d_ptr->m_validationFeedbackMode == ValidationFeedbackMode::ValidatorOnEdit
        || (d_ptr->m_validationFeedbackMode == ValidationFeedbackMode::ValidatorOnCommit
            && d_ptr->m_validationCommitted)) {
        refreshValidationState(false);
    }

    emitValidationStateSignalsIfChanged();

    syncAccessibilityState();

    invalidateLayoutCache();
    syncLineEditGeometry();
    updateGeometry();
    update();
}

QString QtMaterialOutlinedTextField::placeholderText() const
{
    return d_ptr->m_lineEdit ? d_ptr->m_lineEdit->placeholderText() : QString();
}

void QtMaterialOutlinedTextField::setPlaceholderText(const QString& text)
{
    if (!d_ptr->m_lineEdit || d_ptr->m_lineEdit->placeholderText() == text) {
        return;
    }

    d_ptr->m_lineEdit->setPlaceholderText(text);
    syncAccessibilityState();
    syncCharacterCounterWidget();
    update();
}

QString QtMaterialOutlinedTextField::prefixText() const
{
    return d_ptr->m_prefixText;
}

void QtMaterialOutlinedTextField::setPrefixText(const QString& text)
{
    if (d_ptr->m_prefixText == text) {
        return;
    }

    d_ptr->m_prefixText = text;
    invalidateLayoutCache();
    syncLineEditGeometry();
    updateGeometry();
    update();
}

QString QtMaterialOutlinedTextField::suffixText() const
{
    return d_ptr->m_suffixText;
}

void QtMaterialOutlinedTextField::setSuffixText(const QString& text)
{
    if (d_ptr->m_suffixText == text) {
        return;
    }

    d_ptr->m_suffixText = text;
    invalidateLayoutCache();
    syncLineEditGeometry();
    updateGeometry();
    update();
}

QIcon QtMaterialOutlinedTextField::leadingIcon() const
{
    return d_ptr->m_leadingIcon;
}

void QtMaterialOutlinedTextField::setLeadingIcon(const QIcon& icon)
{
    if (d_ptr->m_leadingIcon.cacheKey() == icon.cacheKey()) {
        return;
    }

    d_ptr->m_leadingIcon = icon;
    invalidateLayoutCache();
    syncLineEditGeometry();
    updateGeometry();
    update();
}

QIcon QtMaterialOutlinedTextField::trailingIcon() const
{
    return d_ptr->m_trailingIcon;
}

void QtMaterialOutlinedTextField::setTrailingIcon(const QIcon& icon)
{
    if (d_ptr->m_trailingIcon.cacheKey() == icon.cacheKey()) {
        return;
    }

    d_ptr->m_trailingIcon = icon;
    invalidateLayoutCache();
    syncLineEditGeometry();
    updateGeometry();
    update();
}

bool QtMaterialOutlinedTextField::isClearButtonEnabled() const noexcept
{
    return d_ptr->m_clearButtonEnabled;
}

void QtMaterialOutlinedTextField::setClearButtonEnabled(bool enabled)
{
    if (d_ptr->m_clearButtonEnabled == enabled) {
        return;
    }

    d_ptr->m_clearButtonEnabled = enabled;
    invalidateLayoutCache();
    syncLineEditGeometry();
    updateGeometry();
    update();
}

QLineEdit::EchoMode QtMaterialOutlinedTextField::echoMode() const
{
    return d_ptr->m_configuredEchoMode;
}

void QtMaterialOutlinedTextField::setEchoMode(QLineEdit::EchoMode mode)
{
    if (d_ptr->m_configuredEchoMode == mode) {
        if (mode != QLineEdit::Password && d_ptr->m_passwordVisible) {
            d_ptr->m_passwordVisible = false;
        }
    } else {
        d_ptr->m_configuredEchoMode = mode;
        if (d_ptr->m_configuredEchoMode != QLineEdit::Password) {
            d_ptr->m_passwordVisible = false;
        }
    }

    if (d_ptr->m_lineEdit) {
        d_ptr->m_lineEdit->setEchoMode(
            (d_ptr->m_configuredEchoMode == QLineEdit::Password && d_ptr->m_passwordVisible)
                ? QLineEdit::Normal
                : d_ptr->m_configuredEchoMode);
    }

    invalidateLayoutCache();
    syncLineEditGeometry();
    updateGeometry();
    update();
}

QLineEdit* QtMaterialOutlinedTextField::lineEdit() const
{
    return d_ptr->m_lineEdit;
}

bool QtMaterialOutlinedTextField::isTouched() const noexcept
{
    return m_touched;
}

void QtMaterialOutlinedTextField::setTouched(bool touched)
{
    updateTouchedState(touched);
}

void QtMaterialOutlinedTextField::resetTouched()
{
    setTouched(false);
}

void QtMaterialOutlinedTextField::updateTouchedState(bool touched)
{
    if (m_touched == touched) {
        return;
    }

    m_touched = touched;
    Q_EMIT touchedChanged(touched);
}



const QValidator* QtMaterialOutlinedTextField::validator() const
{
    return d_ptr->m_lineEdit ? d_ptr->m_lineEdit->validator() : nullptr;
}

void QtMaterialOutlinedTextField::setValidator(
    const QValidator* validator)
{
    if (!d_ptr->m_lineEdit
        || d_ptr->m_lineEdit->validator()
            == validator) {
        return;
    }

    d_ptr->m_lineEdit->setValidator(validator);
    refreshValidationState(false);
    syncAccessibilityState();
    invalidateLayoutCache();
    syncLineEditGeometry();
    updateGeometry();
    update();
}

QString QtMaterialOutlinedTextField::inputMask() const
{
    return d_ptr->m_lineEdit ? d_ptr->m_lineEdit->inputMask() : QString();
}

void QtMaterialOutlinedTextField::setInputMask(const QString& inputMask)
{
    if (!d_ptr->m_lineEdit || d_ptr->m_lineEdit->inputMask() == inputMask) {
        return;
    }

    d_ptr->m_lineEdit->setInputMask(inputMask);
    syncCharacterCounterWidget();
    refreshValidationState(false);
    syncAccessibilityState();
    update();
}


bool QtMaterialOutlinedTextField::isReadOnly() const
{
    return d_ptr->m_lineEdit ? d_ptr->m_lineEdit->isReadOnly() : false;
}

void QtMaterialOutlinedTextField::setReadOnly(bool readOnly)
{
    if (!d_ptr->m_lineEdit || d_ptr->m_lineEdit->isReadOnly() == readOnly) {
        return;
    }

    d_ptr->m_lineEdit->setReadOnly(readOnly);

    invalidateLayoutCache();
    syncLineEditGeometry();
    syncAccessibilityState();
    updateGeometry();
    update();
}

bool QtMaterialOutlinedTextField::isRequired() const noexcept
{
    return m_required;
}

void QtMaterialOutlinedTextField::setRequired(bool required)
{
    if (m_required == required) {
        return;
    }

    m_required = required;

    refreshValidationState(false);
    syncAccessibilityState();
    invalidateLayoutCache();
    syncLineEditGeometry();
    updateGeometry();
    update();
}

QString QtMaterialOutlinedTextField::requiredText() const
{
    return m_requiredText.isEmpty() ? tr("Required") : m_requiredText;
}

void QtMaterialOutlinedTextField::setRequiredText(const QString& text)
{
    if (m_requiredText == text) {
        return;
    }

    m_requiredText = text;
    syncAccessibilityState();
    invalidateLayoutCache();
    updateGeometry();
    update();
}


int QtMaterialOutlinedTextField::maxLength() const
{
    return d_ptr->m_lineEdit ? d_ptr->m_lineEdit->maxLength() : 32767;
}

void QtMaterialOutlinedTextField::setMaxLength(
    int maxLength)
{
    if (!d_ptr->m_lineEdit) {
        return;
    }

    const int normalizedMaxLength =
        maxLength <= 0
        ? 32767
        : maxLength;

    if (d_ptr->m_lineEdit->maxLength()
        == normalizedMaxLength) {
        syncCharacterCounterWidget();
        return;
    }

    d_ptr->m_lineEdit->setMaxLength(
        normalizedMaxLength);

    invalidateLayoutCache();
    syncLineEditGeometry();
    syncCharacterCounterWidget();
    syncAccessibilityState();
    updateGeometry();
    update();
}

bool QtMaterialOutlinedTextField::isCharacterCounterEnabled() const noexcept
{
    return m_characterCounterEnabled;
}

void QtMaterialOutlinedTextField::
ensureCharacterCounterLabel()
{
    if (m_characterCounterLabel) {
        return;
    }

    auto* counterLabel = new QLabel(this);
    m_characterCounterLabel = counterLabel;

    counterLabel->setObjectName(
        QStringLiteral(
            "qtmaterial_textfield_characterCounter"));
    counterLabel->setAttribute(
        Qt::WA_TransparentForMouseEvents,
        true);
    counterLabel->setAttribute(
        Qt::WA_NoSystemBackground,
        true);
    counterLabel->setAutoFillBackground(false);
    counterLabel->setAlignment(
        Qt::AlignRight | Qt::AlignVCenter);
    counterLabel->setAccessibleName(
        tr("Character count"));
    counterLabel->hide();
}

void QtMaterialOutlinedTextField::
setCharacterCounterEnabled(bool enabled)
{
    if (m_characterCounterEnabled == enabled) {
        syncCharacterCounterWidget();
        return;
    }

    m_characterCounterEnabled = enabled;
    ensureCharacterCounterLabel();

    invalidateLayoutCache();
    syncLineEditGeometry();
    syncCharacterCounterWidget();
    syncAccessibilityState();
    updateGeometry();
    update();
}

QString QtMaterialOutlinedTextField::characterCounterText() const
{
    return effectiveCharacterCounterText();
}

QString QtMaterialOutlinedTextField::effectiveCharacterCounterText() const
{
    const int currentLength = d_ptr->m_lineEdit ? d_ptr->m_lineEdit->text().size() : 0;
    const int maximumLength = d_ptr->m_lineEdit ? d_ptr->m_lineEdit->maxLength() : 32767;

    if (maximumLength == 32767) {
        return QString::number(currentLength);
    }

    return QStringLiteral("%1/%2").arg(currentLength).arg(maximumLength);
}

void QtMaterialOutlinedTextField::
syncCharacterCounterWidget()
{
    if (d_ptr->m_syncingCounter) {
        return;
    }

    QScopedValueRollback<bool> guard(
        d_ptr->m_syncingCounter,
        true);

    ensureCharacterCounterLabel();

    QLabel* counterLabel =
        m_characterCounterLabel.data();
    if (!counterLabel) {
        return;
    }

    if (!m_characterCounterEnabled) {
        counterLabel->hide();
        return;
    }

    ensureSpecResolved();
    ensureLayoutResolved();

    QFont counterFont =
        spec().hasResolvedSupportingFont
        ? spec().supportingFont
        : font();

    if (!spec().hasResolvedSupportingFont
        && counterFont.pointSizeF() > 0.0) {
        counterFont.setPointSizeF(
            qMax(
                1.0,
                counterFont.pointSizeF() * 0.85));
    }

    counterLabel->setFont(counterFont);
    counterLabel->setText(
        effectiveCharacterCounterText());
    counterLabel->setAccessibleDescription(
        counterLabel->text());

    QPalette counterPalette =
        counterLabel->palette();
    counterPalette.setColor(
        QPalette::WindowText,
        isEnabled()
        ? spec().supportingTextColor
        : spec().disabledSupportingTextColor);
    counterLabel->setPalette(counterPalette);

    const bool shouldShow =
        isVisible()
        && m_cachedCharacterCounterRect.isValid()
        && !m_cachedCharacterCounterRect.isEmpty();

    if (!shouldShow) {
        counterLabel->hide();
        return;
    }

    counterLabel->setGeometry(
        m_cachedCharacterCounterRect);
    counterLabel->show();
    counterLabel->raise();
}

QtMaterialOutlinedTextField::ValidationFeedbackMode
QtMaterialOutlinedTextField::validationFeedbackMode() const noexcept
{
    return d_ptr->m_validationFeedbackMode;
}

void QtMaterialOutlinedTextField::
setValidationFeedbackMode(
    ValidationFeedbackMode mode)
{
    if (d_ptr->m_validationFeedbackMode == mode) {
        return;
    }

    d_ptr->m_validationFeedbackMode = mode;

    if (mode == ValidationFeedbackMode::ValidatorOnEdit) {
        refreshValidationState(false);
    } else {
        d_ptr->m_validationCommitted = false;
        d_ptr->m_automaticValidationError = false;
        m_automaticValidationErrorKind =
            AutomaticValidationErrorKind::None;

        syncEffectiveErrorState();
        syncEffectiveErrorVisibility();
        emitValidationStateSignalsIfChanged();
    }

    syncAccessibilityState();
    invalidateLayoutCache();
    syncLineEditGeometry();
    updateGeometry();
    update();
}

QtMaterialOutlinedTextField::ErrorDisplayMode QtMaterialOutlinedTextField::errorDisplayMode() const noexcept
{
    return m_errorDisplayMode;
}

void QtMaterialOutlinedTextField::setErrorDisplayMode(ErrorDisplayMode mode)
{
    if (m_errorDisplayMode == mode) {
        return;
    }

    m_errorDisplayMode = mode;
    syncEffectiveErrorVisibility();
}

void QtMaterialOutlinedTextField::
showValidationError()
{
    m_errorVisibilityForced = true;
    refreshValidationState(true);
    syncEffectiveErrorVisibility();
}

void QtMaterialOutlinedTextField::resetValidationErrorVisibility()
{
    if (!m_errorVisibilityForced) {
        return;
    }

    m_errorVisibilityForced = false;
    syncEffectiveErrorVisibility();
}


bool QtMaterialOutlinedTextField::validateInput()
{
    m_errorVisibilityForced = true;
    refreshValidationState(true);
    syncEffectiveErrorVisibility();
    syncAccessibilityState();

    invalidateLayoutCache();
    syncLineEditGeometry();
    updateGeometry();
    update();

    const bool acceptable =
        !hasErrorState();
    emit validationRequested(acceptable);
    return acceptable;
}

void QtMaterialOutlinedTextField::
resetValidationFeedback()
{
    const bool hadAutomaticValidationError =
        d_ptr->m_automaticValidationError;
    const bool wasCommitted =
        d_ptr->m_validationCommitted;

    d_ptr->m_validationCommitted = false;
    d_ptr->m_automaticValidationError = false;
    m_automaticValidationErrorKind =
        AutomaticValidationErrorKind::None;

    m_errorVisibilityForced = false;

    syncEffectiveErrorState();
    syncEffectiveErrorVisibility();
    emitValidationStateSignalsIfChanged();
    syncAccessibilityState();

    invalidateLayoutCache();
    syncLineEditGeometry();
    updateGeometry();
    update();

    if (hadAutomaticValidationError
        || wasCommitted) {
        emit validationFeedbackReset();
    }
}

bool QtMaterialOutlinedTextField::isEffectiveErrorVisible() const noexcept
{
    return m_effectiveErrorVisible;
}

bool QtMaterialOutlinedTextField::hasAutomaticValidationError() const noexcept
{
    return d_ptr->m_automaticValidationError;
}

bool QtMaterialOutlinedTextField::isAcceptableInput() const
{
    return !currentValidationError();
}

bool QtMaterialOutlinedTextField::hasErrorState() const noexcept
{
    return d_ptr->m_manualErrorState || d_ptr->m_automaticValidationError;
}

void QtMaterialOutlinedTextField::
setHasErrorState(bool value)
{
    if (d_ptr->m_manualErrorState == value) {
        return;
    }

    d_ptr->m_manualErrorState = value;
    syncEffectiveErrorState();
    syncEffectiveErrorVisibility();
    emitValidationStateSignalsIfChanged();
}

QtMaterialOutlinedTextField::EndActionMode
QtMaterialOutlinedTextField::endActionMode() const noexcept
{
    return d_ptr->m_endActionMode;
}

void QtMaterialOutlinedTextField::setEndActionMode(EndActionMode mode)
{
    if (d_ptr->m_endActionMode == mode) {
        return;
    }

    d_ptr->m_endActionMode = mode;
    invalidateLayoutCache();
    syncLineEditGeometry();
    updateGeometry();
    update();
}

QIcon QtMaterialOutlinedTextField::trailingActionIcon() const
{
    return d_ptr->m_trailingActionIcon;
}

void QtMaterialOutlinedTextField::setTrailingActionIcon(const QIcon& icon)
{
    if (d_ptr->m_trailingActionIcon.cacheKey() == icon.cacheKey()) {
        return;
    }

    d_ptr->m_trailingActionIcon = icon;
    invalidateLayoutCache();
    syncLineEditGeometry();
    updateGeometry();
    update();
}

QString QtMaterialOutlinedTextField::trailingActionText() const
{
    return d_ptr->m_trailingActionText;
}

void QtMaterialOutlinedTextField::setTrailingActionText(const QString& text)
{
    if (d_ptr->m_trailingActionText == text) {
        return;
    }

    d_ptr->m_trailingActionText = text;
    invalidateLayoutCache();
    syncLineEditGeometry();
    updateGeometry();
    update();
}

QString QtMaterialOutlinedTextField::trailingActionToolTip() const
{
    return d_ptr->m_trailingActionToolTip;
}

void QtMaterialOutlinedTextField::setTrailingActionToolTip(const QString& text)
{
    if (d_ptr->m_trailingActionToolTip == text) {
        return;
    }

    d_ptr->m_trailingActionToolTip = text;
    syncAccessoryWidgets();
    update();
}

bool QtMaterialOutlinedTextField::isTrailingActionVisibleWhenEmpty() const noexcept
{
    return d_ptr->m_trailingActionVisibleWhenEmpty;
}

void QtMaterialOutlinedTextField::setTrailingActionVisibleWhenEmpty(bool value)
{
    if (d_ptr->m_trailingActionVisibleWhenEmpty == value) {
        return;
    }

    d_ptr->m_trailingActionVisibleWhenEmpty = value;
    invalidateLayoutCache();
    syncLineEditGeometry();
    updateGeometry();
    update();
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
    d_ptr->m_specDirty = true;
    invalidateLayoutCache();
    syncLineEditPalette();
    syncAccessoryWidgets();
}

void QtMaterialOutlinedTextField::invalidateResolvedSpec()
{
    d_ptr->m_specDirty = true;
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
        d_ptr->m_lineEdit,
        labelText(),
        supportingText(),
        effectiveErrorText(),
        isEffectiveErrorVisible());
}

TextFieldSpec
QtMaterialOutlinedTextField::resolveTextFieldSpec() const
{
    return TextFieldSpecResolver()
        .outlinedTextFieldSpec(
            theme(),
            density());
}

void QtMaterialOutlinedTextField::ensureSpecResolved() const
{
    if (!d_ptr->m_specDirty) {
        return;
    }

    d_ptr->m_spec = resolveTextFieldSpec();
    d_ptr->m_specDirty = false;
}

const TextFieldSpec& QtMaterialOutlinedTextField::spec() const
{
    ensureSpecResolved();
    return d_ptr->m_spec;
}

void QtMaterialOutlinedTextField::invalidateLayoutCache()
{
    d_ptr->m_layoutDirty = true;
}

bool QtMaterialOutlinedTextField::currentFocusState() const
{
    return (d_ptr->m_lineEdit && d_ptr->m_lineEdit->hasFocus()) || hasFocus();
}

QtMaterialOutlinedTextField::EndActionMode QtMaterialOutlinedTextField::resolvedEndActionMode() const noexcept
{
    if (d_ptr->m_endActionMode == EndActionMode::TogglePasswordVisibility
        && d_ptr->m_configuredEchoMode == QLineEdit::Password) {
        return EndActionMode::TogglePasswordVisibility;
    }

    if (d_ptr->m_endActionMode == EndActionMode::ClearText && shouldShowClearAction()) {
        return EndActionMode::ClearText;
    }

    if (d_ptr->m_endActionMode == EndActionMode::CustomTrailingAction
        && shouldShowCustomTrailingAction()) {
        return EndActionMode::CustomTrailingAction;
    }

    return EndActionMode::None;
}

bool QtMaterialOutlinedTextField::shouldShowClearAction() const noexcept
{
    return d_ptr->m_clearButtonEnabled
        && d_ptr->m_lineEdit
        && isEnabled()
        && !d_ptr->m_lineEdit->isReadOnly()
        && (d_ptr->m_trailingActionVisibleWhenEmpty || !d_ptr->m_lineEdit->text().isEmpty());
}

bool QtMaterialOutlinedTextField::shouldShowCustomTrailingAction() const noexcept
{
    return isEnabled()
        && (!d_ptr->m_trailingActionIcon.isNull() || !d_ptr->m_trailingActionText.isEmpty())
        && (d_ptr->m_trailingActionVisibleWhenEmpty || (d_ptr->m_lineEdit && !d_ptr->m_lineEdit->text().isEmpty()));
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

    if (!d_ptr->m_leadingIcon.isNull()) {
        width += iconExtent();
        ++visibleCount;
    }

    if (!d_ptr->m_prefixText.isEmpty()) {
        const int prefixWidth =
            qMax(kMinimumAccessoryTextWidth,
                 fm.horizontalAdvance(d_ptr->m_prefixText) + (2 * kAccessoryTextPadding));
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

    if (!d_ptr->m_suffixText.isEmpty()) {
        const int suffixWidth =
            qMax(kMinimumAccessoryTextWidth,
                 fm.horizontalAdvance(d_ptr->m_suffixText) + (2 * kAccessoryTextPadding));
        width += suffixWidth;
        ++visibleCount;
    }

    int endSlotWidth = 0;
    const EndActionMode resolvedMode = resolvedEndActionMode();

    if (resolvedMode == EndActionMode::ClearText) {
        endSlotWidth = iconExtent() + 6;
    } else if (resolvedMode == EndActionMode::TogglePasswordVisibility) {
        endSlotWidth = fm.horizontalAdvance(d_ptr->m_passwordVisible ? tr("Hide") : tr("Show")) + 16;
    } else if (resolvedMode == EndActionMode::CustomTrailingAction) {
        if (!d_ptr->m_trailingActionIcon.isNull() && !d_ptr->m_trailingActionText.isEmpty()) {
            endSlotWidth = qMax(iconExtent(),
                                fm.horizontalAdvance(d_ptr->m_trailingActionText) + 16);
        } else if (!d_ptr->m_trailingActionIcon.isNull()) {
            endSlotWidth = iconExtent() + 6;
        } else {
            endSlotWidth = fm.horizontalAdvance(d_ptr->m_trailingActionText) + 16;
        }
    } else if (!d_ptr->m_trailingIcon.isNull()) {
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

QtMaterialOutlinedTextField::RequiredValidationMode QtMaterialOutlinedTextField::requiredValidationMode() const noexcept
{
    return m_requiredValidationMode;
}

void QtMaterialOutlinedTextField::setRequiredValidationMode(RequiredValidationMode mode)
{
    if (m_requiredValidationMode == mode) {
        return;
    }

    m_requiredValidationMode = mode;
    refreshValidationState(false);
    syncAccessibilityState();
    invalidateLayoutCache();
    updateGeometry();
    update();
}

QString QtMaterialOutlinedTextField::validatorErrorText() const
{
    return m_validatorErrorText;
}

void QtMaterialOutlinedTextField::setValidatorErrorText(const QString& text)
{
    if (m_validatorErrorText == text) {
        return;
    }

    m_validatorErrorText = text;
    syncAccessibilityState();
    invalidateLayoutCache();
    updateGeometry();
    update();
}

QString QtMaterialOutlinedTextField::inputMaskErrorText() const
{
    return m_inputMaskErrorText;
}

void QtMaterialOutlinedTextField::setInputMaskErrorText(const QString& text)
{
    if (m_inputMaskErrorText == text) {
        return;
    }

    m_inputMaskErrorText = text;
    syncAccessibilityState();
    invalidateLayoutCache();
    updateGeometry();
    update();
}

bool QtMaterialOutlinedTextField::isRequiredValidationError() const
{
    if (!m_required || !d_ptr->m_lineEdit || !isEnabled()) {
        return false;
    }

    const QString value = d_ptr->m_lineEdit->text();

    switch (m_requiredValidationMode) {
    case RequiredValidationMode::NonEmpty:
        return value.isEmpty();
    case RequiredValidationMode::NonBlank:
        return value.trimmed().isEmpty();
    }

    return value.trimmed().isEmpty();
}

QtMaterialOutlinedTextField::AutomaticValidationErrorKind
QtMaterialOutlinedTextField::currentValidationErrorKind() const
{
    if (!d_ptr->m_lineEdit || !isEnabled()) {
        return AutomaticValidationErrorKind::None;
    }

    if (isRequiredValidationError()) {
        return AutomaticValidationErrorKind::Required;
    }

    if (!d_ptr->m_lineEdit->inputMask().isEmpty()) {
        return qtm3InputMaskIsIncomplete(d_ptr->m_lineEdit)
            ? AutomaticValidationErrorKind::InputMask
            : AutomaticValidationErrorKind::None;
    }

    if (d_ptr->m_lineEdit->text().isEmpty()) {
        return AutomaticValidationErrorKind::None;
    }

    if (d_ptr->m_lineEdit->hasAcceptableInput()) {
        return AutomaticValidationErrorKind::None;
    }

    if (d_ptr->m_lineEdit->validator()) {
        return AutomaticValidationErrorKind::Validator;
    }

    return AutomaticValidationErrorKind::Validator;
}

QString QtMaterialOutlinedTextField::effectiveErrorText() const
{
    if (d_ptr->m_manualErrorState && !errorText().isEmpty()) {
        return errorText();
    }

    if (d_ptr->m_automaticValidationError) {
        switch (m_automaticValidationErrorKind) {
        case AutomaticValidationErrorKind::Required:
            return requiredText();
        case AutomaticValidationErrorKind::InputMask:
            if (!m_inputMaskErrorText.isEmpty()) {
                return m_inputMaskErrorText;
            }
            break;
        case AutomaticValidationErrorKind::Validator:
            if (!m_validatorErrorText.isEmpty()) {
                return m_validatorErrorText;
            }
            break;
        case AutomaticValidationErrorKind::None:
        default:
            break;
        }
    }

    if (hasErrorState() && !errorText().isEmpty()) {
        return errorText();
    }

    return hasErrorState() ? tr("Invalid value") : QString();
}

bool QtMaterialOutlinedTextField::currentValidationError() const
{
    return currentValidationErrorKind() != AutomaticValidationErrorKind::None;
}

void QtMaterialOutlinedTextField::
refreshValidationState(bool commit)
{
    if (d_ptr->m_refreshingValidation) {
        return;
    }

    QScopedValueRollback<bool> guard(
        d_ptr->m_refreshingValidation,
        true);

    AutomaticValidationErrorKind errorKind =
        AutomaticValidationErrorKind::None;

    switch (d_ptr->m_validationFeedbackMode) {
    case ValidationFeedbackMode::ManualOnly:
        d_ptr->m_validationCommitted = false;
        break;

    case ValidationFeedbackMode::ValidatorOnEdit:
        d_ptr->m_validationCommitted = true;
        errorKind = currentValidationErrorKind();
        break;

    case ValidationFeedbackMode::ValidatorOnCommit:
        if (commit) {
            d_ptr->m_validationCommitted = true;
        }
        if (d_ptr->m_validationCommitted) {
            errorKind =
                currentValidationErrorKind();
        }
        break;
    }

    m_automaticValidationErrorKind =
        errorKind;
    d_ptr->m_automaticValidationError =
        errorKind
        != AutomaticValidationErrorKind::None;

    syncEffectiveErrorState();
    syncEffectiveErrorVisibility();
    emitValidationStateSignalsIfChanged();
}

void QtMaterialOutlinedTextField::
syncEffectiveErrorState()
{
    QtMaterialInputControl::setHasErrorState(
        d_ptr->m_manualErrorState
        || d_ptr->m_automaticValidationError);
}

void QtMaterialOutlinedTextField::emitValidationStateSignalsIfChanged()
{
    const bool automaticError = d_ptr->m_automaticValidationError;
    const bool effectiveError = d_ptr->m_manualErrorState || d_ptr->m_automaticValidationError;
    const bool acceptableInput = isAcceptableInput();

    if (m_lastEmittedAutomaticValidationError != automaticError) {
        m_lastEmittedAutomaticValidationError = automaticError;
        emit automaticValidationErrorChanged(automaticError);
    }
    if (m_lastEmittedEffectiveErrorState != effectiveError) 
        {        m_lastEmittedEffectiveErrorState = effectiveError;
    emit effectiveErrorStateChanged(effectiveError);
}

if (m_lastEmittedAcceptableInput != acceptableInput) 
    {        m_lastEmittedAcceptableInput = acceptableInput;
emit acceptableInputChanged(acceptableInput);
}
}



bool QtMaterialOutlinedTextField::shouldShowEffectiveError() const noexcept
{
    if (!hasErrorState()) {
        return false;
    }

    // Manual errors are explicit application state, for example server-side
    // validation. They should not be hidden by user-interaction policies.
    if (d_ptr->m_manualErrorState) {
        return true;
    }

    if (m_errorVisibilityForced) {
        return true;
    }

    switch (m_errorDisplayMode) {
    case ErrorDisplayMode::Always:
        return true;
    case ErrorDisplayMode::WhenTouched:
        return isTouched();
    case ErrorDisplayMode::WhenModified:
        return isModified();
    case ErrorDisplayMode::AfterCommit:
        return d_ptr->m_validationCommitted;
    }

    return true;
}

void QtMaterialOutlinedTextField::syncEffectiveErrorVisibility()
{
    const bool visible = shouldShowEffectiveError();
    if (m_effectiveErrorVisible == visible) {
        return;
    }

    m_effectiveErrorVisible = visible;
    syncAccessibilityState();
    invalidateLayoutCache();
    syncLineEditGeometry();
    updateGeometry();
    update();
    emit effectiveErrorVisibleChanged(visible);
}

bool QtMaterialOutlinedTextField::isModified() const
{
    return d_ptr->m_lineEdit ? d_ptr->m_lineEdit->isModified() : false;
}

void QtMaterialOutlinedTextField::setModified(bool modified)
{
    if (!d_ptr->m_lineEdit) {
        return;
    }

    d_ptr->m_lineEdit->setModified(modified);
    updateModifiedStateFromLineEdit();
}

void QtMaterialOutlinedTextField::resetModified()
{
    setModified(false);
}

void QtMaterialOutlinedTextField::updateModifiedStateFromLineEdit()
{
    const bool modified = isModified();
    if (m_lastKnownModified == modified) {
        return;
    }

    m_lastKnownModified = modified;
    Q_EMIT modifiedChanged(modified);
}

void QtMaterialOutlinedTextField::
ensureLayoutResolved() const
{
    ensureSpecResolved();

    if (!d_ptr->m_layoutDirty
        || d_ptr->m_resolvingLayout) {
        return;
    }

    QScopedValueRollback<bool> guard(
        d_ptr->m_resolvingLayout,
        true);

    QtMaterialTextFieldShellHelper::Accessories
        accessories;
    accessories.prefixText =
        d_ptr->m_prefixText;
    accessories.suffixText =
        d_ptr->m_suffixText;
    accessories.customEndActionText =
        d_ptr->m_trailingActionText;
    accessories.hasLeadingIcon =
        !d_ptr->m_leadingIcon.isNull();
    accessories.hasTrailingIcon =
        !d_ptr->m_trailingIcon.isNull();
    accessories.hasCustomEndActionIcon =
        !d_ptr->m_trailingActionIcon.isNull();
    accessories.passwordVisible =
        d_ptr->m_passwordVisible;
    accessories.layoutDirection =
        layoutDirection();
    accessories.iconExtent = iconExtent();
    accessories.accessorySpacing =
        kAccessorySpacing;
    accessories.accessoryTextPadding =
        kAccessoryTextPadding;
    accessories.minimumAccessoryTextWidth =
        kMinimumAccessoryTextWidth;

    switch (resolvedEndActionMode()) {
    case EndActionMode::ClearText:
        accessories.endActionMode =
            QtMaterialTextFieldShellHelper::
                EndActionMode::ClearText;
        break;

    case EndActionMode::TogglePasswordVisibility:
        accessories.endActionMode =
            QtMaterialTextFieldShellHelper::
                EndActionMode::
                    TogglePasswordVisibility;
        break;

    case EndActionMode::CustomTrailingAction:
        accessories.endActionMode =
            QtMaterialTextFieldShellHelper::
                EndActionMode::
                    CustomTrailingAction;
        break;

    case EndActionMode::None:
    default:
        accessories.endActionMode =
            QtMaterialTextFieldShellHelper::
                EndActionMode::None;
        break;
    }

    const QtMaterialTextFieldShellHelper::Variant
        variant =
            shellVariant() == ShellVariant::Filled
            ? QtMaterialTextFieldShellHelper::
                Variant::Filled
            : QtMaterialTextFieldShellHelper::
                Variant::Outlined;

    TextFieldSpec layoutSpec = spec();
    layoutSpec.reserveSupportingLine =
        layoutSpec.reserveSupportingLine
        || m_characterCounterEnabled;

    const QtMaterialTextFieldShellHelper::Layout
        layout =
            QtMaterialTextFieldShellHelper::layoutFor(
                rect(),
                layoutSpec,
                variant,
                accessories,
                font());

    const QtMaterialTextFieldShellHelper::ElidedText
        text =
            QtMaterialTextFieldShellHelper::
                elidedTextFor(
                    layout,
                    layoutSpec,
                    accessories,
                    labelText(),
                    supportingText(),
                    effectiveErrorText(),
                    font());

    d_ptr->m_cachedContainerRect =
        layout.containerRect;
    d_ptr->m_cachedLabelRect =
        layout.labelRect;
    d_ptr->m_cachedEditorRect =
        layout.editorRect;
    d_ptr->m_cachedSupportingRect =
        layout.supportingRect;

    m_cachedCharacterCounterRect = QRect();

    if (m_characterCounterEnabled
        && d_ptr->m_cachedSupportingRect.isValid()) {
        QFont counterFont =
            spec().hasResolvedSupportingFont
            ? spec().supportingFont
            : font();

        if (!spec().hasResolvedSupportingFont
            && counterFont.pointSizeF() > 0.0) {
            counterFont.setPointSizeF(
                qMax(
                    1.0,
                    counterFont.pointSizeF() * 0.85));
        }

        const int counterWidth =
            QFontMetrics(counterFont)
                .horizontalAdvance(
                    effectiveCharacterCounterText())
            + 6;

        const int boundedCounterWidth =
            qMin(
                counterWidth,
                d_ptr->m_cachedSupportingRect.width());

        if (boundedCounterWidth > 0) {
            const bool rtl =
                layoutDirection()
                == Qt::RightToLeft;

            if (rtl) {
                m_cachedCharacterCounterRect =
                    QRect(
                        d_ptr->m_cachedSupportingRect.left(),
                        d_ptr->m_cachedSupportingRect.top(),
                        boundedCounterWidth,
                        d_ptr->m_cachedSupportingRect.height());
                d_ptr->m_cachedSupportingRect.setLeft(
                    m_cachedCharacterCounterRect.right()
                    + 4);
            } else {
                m_cachedCharacterCounterRect =
                    QRect(
                        d_ptr->m_cachedSupportingRect.right()
                            - boundedCounterWidth
                            + 1,
                        d_ptr->m_cachedSupportingRect.top(),
                        boundedCounterWidth,
                        d_ptr->m_cachedSupportingRect.height());
                d_ptr->m_cachedSupportingRect.setRight(
                    m_cachedCharacterCounterRect.left()
                    - 4);
            }
        }
    }

    d_ptr->m_cachedFocusRect =
        layout.focusRect;
    d_ptr->m_cachedLeadingIconRect =
        layout.leadingIconRect;
    d_ptr->m_cachedPrefixRect =
        layout.prefixRect;
    d_ptr->m_cachedSuffixRect =
        layout.suffixRect;
    d_ptr->m_cachedTrailingIconRect =
        layout.trailingIconRect;
    d_ptr->m_cachedEndActionRect =
        layout.endActionRect;
    d_ptr->m_cachedRadius =
        layout.radius;

    d_ptr->m_cachedLabelText =
        text.labelText;
    d_ptr->m_cachedSupportingText =
        text.supportingText;
    d_ptr->m_cachedErrorText =
        text.errorText;
    d_ptr->m_cachedPrefixText =
        text.prefixText;
    d_ptr->m_cachedSuffixText =
        text.suffixText;
    d_ptr->m_cachedEndActionText =
        text.endActionText;
    d_ptr->m_cachedDisplaySupportingText =
        isEffectiveErrorVisible()
        ? text.errorText
        : text.supportingText;

    d_ptr->m_cachedLabelFont =
        spec().hasResolvedLabelFont
        ? spec().labelFont
        : font();
    d_ptr->m_cachedSupportingFont =
        spec().hasResolvedSupportingFont
        ? spec().supportingFont
        : font();

    d_ptr->m_cachedContainerPath =
        QPainterPath();
    d_ptr->m_cachedContainerPath
        .addRoundedRect(
            d_ptr->m_cachedContainerRect,
            d_ptr->m_cachedRadius,
            d_ptr->m_cachedRadius);

    d_ptr->m_layoutDirty = false;
}

void QtMaterialOutlinedTextField::
syncLineEditGeometry()
{
    if (d_ptr->m_syncingGeometry) {
        return;
    }

    QScopedValueRollback<bool> guard(
        d_ptr->m_syncingGeometry,
        true);

    ensureLayoutResolved();

    if (d_ptr->m_lineEdit
        && d_ptr->m_lineEdit->geometry()
            != d_ptr->m_cachedEditorRect) {
        d_ptr->m_lineEdit->setGeometry(
            d_ptr->m_cachedEditorRect);
    }

    syncCharacterCounterWidget();
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

    applyTextPalette(d_ptr->m_prefixLabel, textColor);
    applyTextPalette(d_ptr->m_suffixLabel, textColor);

    if (d_ptr->m_prefixLabel) {
        d_ptr->m_prefixLabel->setFont(font());
        if (d_ptr->m_cachedPrefixRect.isEmpty() || d_ptr->m_cachedPrefixText.isEmpty()) {
            d_ptr->m_prefixLabel->hide();
        } else {
            d_ptr->m_prefixLabel->setGeometry(d_ptr->m_cachedPrefixRect);
            d_ptr->m_prefixLabel->setAlignment(Qt::AlignVCenter | (rtl ? Qt::AlignRight : Qt::AlignLeft));
            d_ptr->m_prefixLabel->setText(d_ptr->m_cachedPrefixText);
            d_ptr->m_prefixLabel->show();
        }
    }

    if (d_ptr->m_suffixLabel) {
        d_ptr->m_suffixLabel->setFont(font());
        if (d_ptr->m_cachedSuffixRect.isEmpty() || d_ptr->m_cachedSuffixText.isEmpty()) {
            d_ptr->m_suffixLabel->hide();
        } else {
            d_ptr->m_suffixLabel->setGeometry(d_ptr->m_cachedSuffixRect);
            d_ptr->m_suffixLabel->setAlignment(Qt::AlignVCenter | (rtl ? Qt::AlignRight : Qt::AlignLeft));
            d_ptr->m_suffixLabel->setText(d_ptr->m_cachedSuffixText);
            d_ptr->m_suffixLabel->show();
        }
    }

    const int iconSize = iconExtent();

    if (d_ptr->m_leadingIconLabel) {
        if (d_ptr->m_cachedLeadingIconRect.isEmpty() || d_ptr->m_leadingIcon.isNull()) {
            d_ptr->m_leadingIconLabel->hide();
        } else {
            d_ptr->m_leadingIconLabel->setPixmap(
                d_ptr->m_leadingIcon.pixmap(QSize(iconSize, iconSize),
                                     isEnabled() ? QIcon::Normal : QIcon::Disabled));
            d_ptr->m_leadingIconLabel->setGeometry(d_ptr->m_cachedLeadingIconRect);
            d_ptr->m_leadingIconLabel->show();
        }
    }

    if (d_ptr->m_trailingIconLabel) {
        if (d_ptr->m_cachedTrailingIconRect.isEmpty() || d_ptr->m_trailingIcon.isNull()) {
            d_ptr->m_trailingIconLabel->hide();
        } else {
            d_ptr->m_trailingIconLabel->setPixmap(
                d_ptr->m_trailingIcon.pixmap(QSize(iconSize, iconSize),
                                      isEnabled() ? QIcon::Normal : QIcon::Disabled));
            d_ptr->m_trailingIconLabel->setGeometry(d_ptr->m_cachedTrailingIconRect);
            d_ptr->m_trailingIconLabel->show();
        }
    }

    if (d_ptr->m_endActionButton) {
        QPalette palette = d_ptr->m_endActionButton->palette();
        palette.setColor(QPalette::ButtonText, secondaryTextColor);
        d_ptr->m_endActionButton->setPalette(palette);
        d_ptr->m_endActionButton->setEnabled(isEnabled());
        d_ptr->m_endActionButton->setFont(font());

        const EndActionMode mode = resolvedEndActionMode();

        if (d_ptr->m_cachedEndActionRect.isEmpty() || mode == EndActionMode::None) {
            d_ptr->m_endActionButton->setAccessibleName(QString());
            d_ptr->m_endActionButton->setAccessibleDescription(QString());
            d_ptr->m_endActionButton->hide();
        } else {
            d_ptr->m_endActionButton->setGeometry(d_ptr->m_cachedEndActionRect);
            d_ptr->m_endActionButton->setIcon(QIcon());
            d_ptr->m_endActionButton->setText(QString());
            d_ptr->m_endActionButton->setToolTip(QString());

            if (mode == EndActionMode::ClearText) {
                const QIcon clearIcon = style()->standardIcon(QStyle::SP_LineEditClearButton);
                if (clearIcon.isNull()) {
                    d_ptr->m_endActionButton->setText(QStringLiteral("×"));
                } else {
                    d_ptr->m_endActionButton->setIcon(clearIcon);
                    d_ptr->m_endActionButton->setIconSize(QSize(iconSize, iconSize));
                }
                d_ptr->m_endActionButton->setToolTip(tr("Clear text"));
                d_ptr->m_endActionButton->setAccessibleName(tr("Clear text"));
                d_ptr->m_endActionButton->setAccessibleDescription(tr("Clear text"));
            } else if (mode == EndActionMode::TogglePasswordVisibility) {
                const QString passwordActionText =
                    d_ptr->m_passwordVisible ? tr("Hide password") : tr("Show password");
                d_ptr->m_endActionButton->setText(d_ptr->m_cachedEndActionText);
                d_ptr->m_endActionButton->setToolTip(passwordActionText);
                d_ptr->m_endActionButton->setAccessibleName(passwordActionText);
                d_ptr->m_endActionButton->setAccessibleDescription(passwordActionText);
            } else if (mode == EndActionMode::CustomTrailingAction) {
                if (!d_ptr->m_trailingActionIcon.isNull()) {
                    d_ptr->m_endActionButton->setIcon(d_ptr->m_trailingActionIcon);
                    d_ptr->m_endActionButton->setIconSize(QSize(iconSize, iconSize));
                }
                if (!d_ptr->m_cachedEndActionText.isEmpty()) {
                    d_ptr->m_endActionButton->setText(d_ptr->m_cachedEndActionText);
                }
                d_ptr->m_endActionButton->setToolTip(d_ptr->m_trailingActionToolTip);
                const QString customAccessibleName = !d_ptr->m_trailingActionText.isEmpty()
                    ? d_ptr->m_trailingActionText
                    : (!d_ptr->m_trailingActionToolTip.isEmpty()
                           ? d_ptr->m_trailingActionToolTip
                           : tr("Text field action"));
                d_ptr->m_endActionButton->setAccessibleName(customAccessibleName);
                d_ptr->m_endActionButton->setAccessibleDescription(d_ptr->m_trailingActionToolTip);
            }

            d_ptr->m_endActionButton->show();
        }
    }
}

void QtMaterialOutlinedTextField::syncLineEditPalette()
{
    if (!d_ptr->m_lineEdit) {
        return;
    }

    ensureSpecResolved();

    QPalette palette = d_ptr->m_lineEdit->palette();
    palette.setColor(
        QPalette::Text,
        isEnabled() ? spec().inputTextColor : spec().disabledInputTextColor);
    palette.setColor(
        QPalette::PlaceholderText,
        isEnabled() ? spec().labelColor : spec().disabledLabelColor);
    palette.setColor(QPalette::Base, Qt::transparent);
    palette.setColor(QPalette::Window, Qt::transparent);
    d_ptr->m_lineEdit->setPalette(palette);

    syncAccessoryWidgets();
}

void QtMaterialOutlinedTextField::resizeEvent(QResizeEvent* event)
{
    QtMaterialInputControl::resizeEvent(event);
    invalidateLayoutCache();
    syncLineEditGeometry();
    syncAccessibilityState();
}

void QtMaterialOutlinedTextField::showEvent(QShowEvent* event)
{
    QtMaterialInputControl::showEvent(event);

    invalidateLayoutCache();
    syncLineEditGeometry();
    syncAccessoryWidgets();
    syncCharacterCounterWidget();
    syncAccessibilityState();

    updateGeometry();
    update();
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
    if (watched == d_ptr->m_lineEdit) {
        switch (event->type()) {
        case QEvent::FocusIn:
            interactionState().setFocused(true);
            if (d_ptr->m_transition) {
                d_ptr->m_transition->startForward();
            }
            update();
            break;

        case QEvent::FocusOut:
            interactionState().setFocused(false);
            if (d_ptr->m_transition) {
                d_ptr->m_transition->startBackward();
            }
            updateTouchedState(true);
            if (d_ptr->m_validationFeedbackMode == ValidationFeedbackMode::ValidatorOnCommit) {
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

    QtMaterialInteractionState state = interactionState();
    state.setFocused(currentFocusState());
    state.setEnabled(isEnabled());
    state.setError(isEffectiveErrorVisible());

    QPainter painter(this);

    QtMaterialTextFieldShellHelper::Layout layout;
    layout.containerRect = d_ptr->m_cachedContainerRect;
    layout.labelRect = d_ptr->m_cachedLabelRect;
    layout.editorRect = d_ptr->m_cachedEditorRect;
    layout.supportingRect = d_ptr->m_cachedSupportingRect;
    layout.focusRect = d_ptr->m_cachedFocusRect;
    layout.leadingIconRect = d_ptr->m_cachedLeadingIconRect;
    layout.prefixRect = d_ptr->m_cachedPrefixRect;
    layout.suffixRect = d_ptr->m_cachedSuffixRect;
    layout.trailingIconRect = d_ptr->m_cachedTrailingIconRect;
    layout.endActionRect = d_ptr->m_cachedEndActionRect;
    layout.layoutDirection = layoutDirection();
    layout.radius = d_ptr->m_cachedRadius;

    QtMaterialTextFieldShellHelper::ElidedText text;
    text.labelText = d_ptr->m_cachedLabelText;
    text.supportingText = d_ptr->m_cachedSupportingText;
    text.errorText = d_ptr->m_cachedErrorText;
    text.prefixText = d_ptr->m_cachedPrefixText;
    text.suffixText = d_ptr->m_cachedSuffixText;
    text.endActionText = d_ptr->m_cachedEndActionText;

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
        isEffectiveErrorVisible(),
        d_ptr->m_cachedLabelFont);
}

} // namespace QtMaterial