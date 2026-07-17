#pragma once
#include <memory>

#include <QFont>
#include <QIcon>
#include <QLineEdit>
#include <QPointer>
#include <QValidator>

#include "qtmaterial/core/qtmaterialinputcontrol.h"
#include "qtmaterial/specs/qtmaterialtextfieldspec.h"
#include "qtmaterial/qtmaterialglobal.h"

class QEvent;
class QLabel;
class QResizeEvent;
class QShowEvent;
class QToolButton;

namespace QtMaterial {
class QtMaterialOutlinedTextFieldPrivate;

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
    void showEvent(QShowEvent *event) override;
    void resizeEvent(QResizeEvent* event) override;
    void changeEvent(QEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void invalidateResolvedSpec() override;
    void contentChangedEvent() override;
    virtual ShellVariant shellVariant() const;
    void syncAccessibilityState() override;
    virtual TextFieldSpec resolveTextFieldSpec() const;

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
    std::unique_ptr<QtMaterialOutlinedTextFieldPrivate> d_ptr;

    mutable QRect m_cachedCharacterCounterRect;
    QPointer<QLabel> m_characterCounterLabel;
    RequiredValidationMode m_requiredValidationMode = RequiredValidationMode::NonBlank;
    QString m_validatorErrorText;
    QString m_inputMaskErrorText;
    AutomaticValidationErrorKind m_automaticValidationErrorKind = AutomaticValidationErrorKind::None;
    ErrorDisplayMode m_errorDisplayMode = ErrorDisplayMode::Always;
    bool m_errorVisibilityForced = false;
    bool m_effectiveErrorVisible = false;
    bool m_lastEmittedAutomaticValidationError = false;
    bool m_lastEmittedEffectiveErrorState = false;
    bool m_lastEmittedAcceptableInput = true;    
    bool m_required = false;
    QString m_requiredText;

    bool m_characterCounterEnabled = false;
    bool m_lastKnownModified = false;
    bool m_touched = false;
};

} // namespace QtMaterial