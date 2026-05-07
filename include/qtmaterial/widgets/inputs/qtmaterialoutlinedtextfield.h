#pragma once

#include <QFont>
#include <QIcon>
#include <QLineEdit>
#include <QPainterPath>
#include <QPointer>
#include <QValidator>

#include "qtmaterial/core/qtmaterialinputcontrol.h"
#include "qtmaterial/specs/qtmaterialtextfieldspec.h"
#include "qtmaterial/qtmaterialglobal.h"

class QEvent;
class QLabel;
class QResizeEvent;
class QToolButton;

namespace QtMaterial {

class QtMaterialTransitionController;
class SpecFactory;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialOutlinedTextField : public QtMaterialInputControl
{
    Q_OBJECT

public:
    enum class ValidationFeedbackMode {
        ManualOnly,
        ValidatorOnEdit,
        ValidatorOnCommit,
    };

    enum class RequiredValidationMode {
        NonEmpty,
        NonBlank,
    };

    enum class ErrorDisplayMode {
        Always,
        WhenTouched,
        WhenModified,
        AfterCommit,
    };

    enum class EndActionMode {
        None,
        ClearText,
        TogglePasswordVisibility,
        CustomTrailingAction,
    };

    explicit QtMaterialOutlinedTextField(QWidget* parent = nullptr);
    ~QtMaterialOutlinedTextField() override;

    QString text() const;
    void setText(const QString& text);

    QString placeholderText() const;
    void setPlaceholderText(const QString& text);

    QString prefixText() const;
    void setPrefixText(const QString& text);

    QString suffixText() const;
    void setSuffixText(const QString& text);

    QIcon leadingIcon() const;
    void setLeadingIcon(const QIcon& icon);

    QIcon trailingIcon() const;
    void setTrailingIcon(const QIcon& icon);

    bool isClearButtonEnabled() const noexcept;
    void setClearButtonEnabled(bool enabled);

    QLineEdit::EchoMode echoMode() const;
    void setEchoMode(QLineEdit::EchoMode mode);

    QLineEdit* lineEdit() const;

    bool isModified() const;
    void setModified(bool modified);
    void resetModified();

    bool isTouched() const noexcept;
    void setTouched(bool touched);
    void resetTouched();

    const QValidator* validator() const;
    void setValidator(const QValidator* validator);

    QString inputMask() const;
    void setInputMask(const QString& inputMask);

    bool isRequired() const noexcept;
    void setRequired(bool required);
    QString requiredText() const;
    void setRequiredText(const QString& text);

    bool isReadOnly() const;
    void setReadOnly(bool readOnly);

    int maxLength() const;
    void setMaxLength(int maxLength);

    bool isCharacterCounterEnabled() const noexcept;
    void setCharacterCounterEnabled(bool enabled);
    QString characterCounterText() const;


    ValidationFeedbackMode validationFeedbackMode() const noexcept;
    void setValidationFeedbackMode(ValidationFeedbackMode mode);
    RequiredValidationMode requiredValidationMode() const noexcept;
    void setRequiredValidationMode(RequiredValidationMode mode);
    QString validatorErrorText() const;
    void setValidatorErrorText(const QString& text);
    QString inputMaskErrorText() const;
    void setInputMaskErrorText(const QString& text);

    ErrorDisplayMode errorDisplayMode() const noexcept;
    void setErrorDisplayMode(ErrorDisplayMode mode);
    void showValidationError();
    void resetValidationErrorVisibility();
    bool isEffectiveErrorVisible() const noexcept;
    bool validateInput();
    void resetValidationFeedback();

    bool hasAutomaticValidationError() const noexcept;
    bool isAcceptableInput() const;

    bool hasErrorState() const noexcept;
    void setHasErrorState(bool value);

    EndActionMode endActionMode() const noexcept;
    void setEndActionMode(EndActionMode mode);

    QIcon trailingActionIcon() const;
    void setTrailingActionIcon(const QIcon& icon);

    QString trailingActionText() const;
    void setTrailingActionText(const QString& text);

    QString trailingActionToolTip() const;
    void setTrailingActionToolTip(const QString& text);

    bool isTrailingActionVisibleWhenEmpty() const noexcept;
    void setTrailingActionVisibleWhenEmpty(bool value);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    QString effectiveErrorText() const;

signals:
    void trailingActionTriggered();
    void clearTriggered();
    void passwordVisibilityChanged(bool visible);
    void automaticValidationErrorChanged(bool hasError);
    void effectiveErrorStateChanged(bool hasError);
    void acceptableInputChanged(bool acceptable);    

Q_SIGNALS:
    void modifiedChanged(bool modified);
    void touchedChanged(bool touched);

    void effectiveErrorVisibleChanged(bool visible);
    void validationRequested(bool acceptable);
    void validationFeedbackReset();

protected:
    enum class ShellVariant {
        Outlined,
        Filled,
    };

    enum class AutomaticValidationErrorKind {
        None,
        Required,
        Validator,
        InputMask,
    };

    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void changeEvent(QEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void invalidateResolvedSpec() override;
    void contentChangedEvent() override;
    virtual ShellVariant shellVariant() const;
    void syncAccessibilityState() override;
    virtual TextFieldSpec resolveTextFieldSpec(const SpecFactory& factory) const;

    void ensureSpecResolved() const;
    const TextFieldSpec& spec() const;

private:
    void invalidateLayoutCache();
    void updateModifiedStateFromLineEdit();
    void updateTouchedState(bool touched);
    void ensureLayoutResolved() const;
    void syncLineEditGeometry();
    void syncAccessoryWidgets();

    void syncCharacterCounterWidget();
    QString effectiveCharacterCounterText() const;
    void syncLineEditPalette();
    bool currentFocusState() const;

    EndActionMode resolvedEndActionMode() const noexcept;
    bool shouldShowClearAction() const noexcept;
    bool shouldShowCustomTrailingAction() const noexcept;

    int iconExtent() const;
    int effectiveLeadingReserve() const;
    int effectiveTrailingReserve() const;

    AutomaticValidationErrorKind currentValidationErrorKind() const;
    bool currentValidationError() const;
    bool isRequiredValidationError() const;
    void refreshValidationState(bool commit);
    void syncEffectiveErrorState();
    bool shouldShowEffectiveError() const noexcept;
    void syncEffectiveErrorVisibility();
    void emitValidationStateSignalsIfChanged();

    mutable bool m_specDirty = true;
    mutable bool m_layoutDirty = true;
    mutable TextFieldSpec m_spec;

    mutable QRect m_cachedContainerRect;
    mutable QRect m_cachedLabelRect;
    mutable QRect m_cachedEditorRect;
    mutable QRect m_cachedSupportingRect;
    mutable QRect m_cachedCharacterCounterRect;
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
    QPointer<QLabel> m_characterCounterLabel;

    QString m_prefixText;
    QString m_suffixText;
    QIcon m_leadingIcon;
    QIcon m_trailingIcon;
    bool m_clearButtonEnabled = false;
    QLineEdit::EchoMode m_configuredEchoMode = QLineEdit::Normal;
    bool m_passwordVisible = false;
    RequiredValidationMode m_requiredValidationMode = RequiredValidationMode::NonBlank;
    QString m_validatorErrorText;
    QString m_inputMaskErrorText;
    AutomaticValidationErrorKind m_automaticValidationErrorKind = AutomaticValidationErrorKind::None;

    ValidationFeedbackMode m_validationFeedbackMode = ValidationFeedbackMode::ManualOnly;
    ErrorDisplayMode m_errorDisplayMode = ErrorDisplayMode::Always;
    bool m_errorVisibilityForced = false;
    bool m_effectiveErrorVisible = false;
    bool m_manualErrorState = false;
    bool m_automaticValidationError = false;
    bool m_validationCommitted = false;
    bool m_lastEmittedAutomaticValidationError = false;
    bool m_lastEmittedEffectiveErrorState = false;
    bool m_lastEmittedAcceptableInput = true;    
    bool m_required = false;
    QString m_requiredText;

    EndActionMode m_endActionMode = EndActionMode::None;
    QIcon m_trailingActionIcon;
    QString m_trailingActionText;
    QString m_trailingActionToolTip;
    bool m_trailingActionVisibleWhenEmpty = false;

    bool m_characterCounterEnabled = false;

    QtMaterialTransitionController* m_transition = nullptr;
    bool m_lastKnownModified = false;
    bool m_touched = false;
};

} // namespace QtMaterial