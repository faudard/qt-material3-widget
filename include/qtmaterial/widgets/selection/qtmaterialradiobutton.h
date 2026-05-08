#pragma once

#include <memory>

#include "qtmaterial/core/qtmaterialselectioncontrol.h"
#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {
class QtMaterialRadioButtonPrivate;

struct QtMaterialRadioButtonPrivate;
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialRadioButton : public QtMaterialSelectionControl
{
    Q_OBJECT
public:
    explicit QtMaterialRadioButton(QWidget* parent = nullptr);
    explicit QtMaterialRadioButton(const QString& text, QWidget* parent = nullptr);
    ~QtMaterialRadioButton() override;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void changeEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void invalidateResolvedSpec() override;
    void stateChangedEvent() override;

private:
    void resolveSpecIfNeeded() const;
    void invalidateLayoutCache();
    void resolveLayoutIfNeeded() const;
    std::unique_ptr<QtMaterialRadioButtonPrivate> d;
};

} // namespace QtMaterial
