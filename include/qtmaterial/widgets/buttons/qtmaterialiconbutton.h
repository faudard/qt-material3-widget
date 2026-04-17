#pragma once
#include <QIcon>
#include "qtmaterial/core/qtmaterialabstractbutton.h"
#include "qtmaterial/specs/qtmaterialiconbuttonspec.h"
#include "qtmaterial/qtmaterialglobal.h"
namespace QtMaterial {
class QtMaterialRippleController;
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialIconButton : public QtMaterialAbstractButton
{
    Q_OBJECT
public:
    explicit QtMaterialIconButton(QWidget* parent = nullptr);
    ~QtMaterialIconButton() override;

    QIcon icon() const;
    void setIcon(const QIcon& icon);
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void invalidateResolvedSpec() override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    void resolveSpecIfNeeded() const;
    mutable bool m_specDirty = true;
    mutable IconButtonSpec m_spec;
    QIcon m_icon;
    QtMaterialRippleController* m_ripple = nullptr;
};
} // namespace QtMaterial
