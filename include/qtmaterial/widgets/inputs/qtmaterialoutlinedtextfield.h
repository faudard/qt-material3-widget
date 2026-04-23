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

    const QValidator* validator() const;
    void setValidator(const QValidator* validator);

    QString inputMask() const;
    void setInputMask(const QString& inputMask);

    ValidationFeedbackMode validationFeedbackMode() const noexcept;
    void setValidationFeedbackMode(ValidationFeedbackMode mode);

    bool hasAutomaticValidationError() const noexcept;

    bool hasErrorState() const noexcept;
    void setHasErrorState(bool value);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    enum class ShellVariant {
        Outlined,
        Filled,
    };

    enum class EndActionMode {
        None,
        ClearText,
        TogglePasswordVisibility,
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
    void ensureLayoutResolved() const;
    void syncLineEditGeometry();
    void syncAccessoryWidgets();
    void syncLineEditPalette();
    bool currentFocusState() const;

    EndActionMode currentEndActionMode() const noexcept;
    int iconExtent() const;
    int effectiveLeadingReserve() const;
    int effectiveTrailingReserve() const;

    bool currentValidationError() const;
    void refreshValidationState(bool commit);
    void syncEffectiveErrorState();

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

    ValidationFeedbackMode m_validationFeedbackMode = ValidationFeedbackMode::ManualOnly;
    bool m_manualErrorState = false;
    bool m_automaticValidationError = false;
    bool m_validationCommitted = false;

    QtMaterialTransitionController* m_transition = nullptr;
};

} // namespace QtMaterial