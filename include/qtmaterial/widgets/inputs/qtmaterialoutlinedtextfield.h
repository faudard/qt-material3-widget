#pragma once
#include <QPointer>
#include "qtmaterial/core/qtmaterialinputcontrol.h"
#include "qtmaterial/specs/qtmaterialtextfieldspec.h"
#include "qtmaterial/qtmaterialglobal.h"
class QLineEdit;
namespace QtMaterial {
class QtMaterialTransitionController;
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialOutlinedTextField : public QtMaterialInputControl
{
    Q_OBJECT
public:
    explicit QtMaterialOutlinedTextField(QWidget* parent = nullptr);
    ~QtMaterialOutlinedTextField() override;

    QString text() const;
    void setText(const QString& text);
    QLineEdit* lineEdit() const;

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void invalidateResolvedSpec() override;

private:
    void resolveSpecIfNeeded() const;

    mutable bool m_specDirty = true;
    mutable TextFieldSpec m_spec;
    QPointer<QLineEdit> m_lineEdit;
    QtMaterialTransitionController* m_transition = nullptr;
};
} // namespace QtMaterial
