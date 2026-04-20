#pragma once

#include <QPointer>

#include "qtmaterial/core/qtmaterialinputcontrol.h"
#include "qtmaterial/specs/qtmaterialtextfieldspec.h"
#include "qtmaterial/qtmaterialglobal.h"

class QLineEdit;

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
    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void invalidateResolvedSpec() override;

    virtual ShellVariant shellVariant() const;
    virtual TextFieldSpec resolveTextFieldSpec(const SpecFactory& factory) const;
    void ensureSpecResolved() const;
    const TextFieldSpec& spec() const;

private:
    mutable bool m_specDirty = true;
    mutable TextFieldSpec m_spec;
    QPointer<QLineEdit> m_lineEdit;
    QtMaterialTransitionController* m_transition = nullptr;
};

} // namespace QtMaterial
