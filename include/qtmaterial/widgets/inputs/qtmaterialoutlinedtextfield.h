#pragma once

#include <QPainterPath>
#include <QPointer>
#include <QRect>

#include "qtmaterial/core/qtmaterialinputcontrol.h"
#include "qtmaterial/specs/qtmaterialtextfieldspec.h"
#include "qtmaterial/qtmaterialglobal.h"

class QLineEdit;
class QEvent;
class QResizeEvent;

namespace QtMaterial {

class QtMaterialTransitionController;
class SpecFactory;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialOutlinedTextField : public QtMaterialInputControl
{
    Q_OBJECT
public:
    explicit QtMaterialOutlinedTextField(QWidget* parent = nullptr);
    ~QtMaterialOutlinedTextField() override;

    QString text() const;
    void setText(const QString& text);
    QLineEdit* lineEdit() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    enum class ShellVariant {
        Outlined,
        Filled,
    };

    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void changeEvent(QEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void invalidateResolvedSpec() override;

    virtual ShellVariant shellVariant() const;
    void syncAccessibilityState() override;
    virtual TextFieldSpec resolveTextFieldSpec(const SpecFactory& factory) const;
    void ensureSpecResolved() const;
    const TextFieldSpec& spec() const;

private:
    void invalidateLayoutCache();
    void ensureLayoutResolved() const;
    void syncLineEditGeometry();
    void syncLineEditPalette();
    bool currentFocusState() const;

    mutable bool m_specDirty = true;
    mutable bool m_layoutDirty = true;
    mutable TextFieldSpec m_spec;
    mutable QRect m_cachedContainerRect;
    mutable QRect m_cachedLabelRect;
    mutable QRect m_cachedEditorRect;
    mutable QRect m_cachedSupportingRect;
    mutable QRect m_cachedFocusRect;
    mutable qreal m_cachedRadius = 0.0;
    mutable QString m_cachedLabelText;
    mutable QString m_cachedSupportingText;
    mutable QString m_cachedErrorText;
    mutable QString m_cachedDisplaySupportingText;
    mutable QPainterPath m_cachedContainerPath;
    QPointer<QLineEdit> m_lineEdit;
    QtMaterialTransitionController* m_transition = nullptr;
};

} // namespace QtMaterial
