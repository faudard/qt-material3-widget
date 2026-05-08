#pragma once

#include <memory>
#include "qtmaterial/core/qtmaterialselectioncontrol.h"
#include "qtmaterial/qtmaterialglobal.h"

class QKeyEvent;
class QMouseEvent;
class QResizeEvent;

namespace QtMaterial {

class QtMaterialSwitchPrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialSwitch : public QtMaterialSelectionControl
{
    Q_OBJECT
public:
    explicit QtMaterialSwitch(QWidget* parent = nullptr);
    explicit QtMaterialSwitch(const QString& text, QWidget* parent = nullptr);
    ~QtMaterialSwitch() override;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void changeEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void invalidateResolvedSpec() override;
    void stateChangedEvent() override;
    void contentChangedEvent() override;

private:
    void resolveSpecIfNeeded() const;
    void invalidateLayoutCache();
    void resolveLayoutIfNeeded() const;
    void syncTransitionState(bool animated);
    void syncAccessibleState();
 std::unique_ptr<QtMaterialSwitchPrivate> d;
};

} // namespace QtMaterial
