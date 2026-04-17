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
    QString placeholderText() const;
    void setPlaceholderText(const QString& text);
    QLineEdit* lineEdit() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void changeEvent(QEvent* event) override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void invalidateResolvedSpec() override;
    void stateChangedEvent() override;

private:
    void resolveSpecIfNeeded() const;
    void updateLineEditStyle();
    void updateTransitionTarget();
    QRectF containerRect() const;
    QRect editorRect() const;
    QRect supportingTextRectForSpec() const;
    QRectF labelRectForProgress(qreal progress) const;
    qreal stateLayerOpacity() const;

    mutable bool m_specDirty = true;
    mutable TextFieldSpec m_spec;
    QPointer<QLineEdit> m_lineEdit;
    QtMaterialTransitionController* m_transition = nullptr;
};
} // namespace QtMaterial
