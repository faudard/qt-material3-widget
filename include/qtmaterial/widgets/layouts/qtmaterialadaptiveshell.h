#pragma once

#include <memory>

#include "qtmaterial/foundation/qtmaterialdensity.h"
#include "qtmaterial/foundation/qtmaterialwindowsizeclass.h"
#include "qtmaterial/core/qtmaterialwidget.h"

class QEvent;
class QResizeEvent;

namespace QtMaterial {

class QtMaterialAdaptiveShellPrivate;
class QtMaterialNavigationSuite;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialAdaptiveShell : public QtMaterialWidget
{
    Q_OBJECT

public:
    explicit QtMaterialAdaptiveShell(QWidget* parent = nullptr);
    ~QtMaterialAdaptiveShell() override;

    QtMaterialNavigationSuite* navigationSuite() const noexcept;

    QWidget* contentWidget() const noexcept;
    void setContentWidget(QWidget* widget);

    QWidget* supportingWidget() const noexcept;
    void setSupportingWidget(QWidget* widget);

    WindowSizeClass windowSizeClass() const noexcept;
    Density resolvedDensity() const noexcept;

    bool automaticDensity() const noexcept;
    void setAutomaticDensity(bool enabled);

    int supportingPaneWidth() const noexcept;
    void setSupportingPaneWidth(int width);

signals:
    void widthSizeClassChanged(QtMaterial::WindowWidthSizeClass sizeClass);
    void heightSizeClassChanged(QtMaterial::WindowHeightSizeClass sizeClass);
    void resolvedDensityChanged(QtMaterial::Density density);
    void automaticDensityChanged(bool enabled);

protected:
    void resizeEvent(QResizeEvent* event) override;
    void changeEvent(QEvent* event) override;

private:
    void applyAdaptiveState();
    void applyAdaptiveGeometry();
    void applyDensityToChildren();

    std::unique_ptr<QtMaterialAdaptiveShellPrivate> d_ptr;
};

} // namespace QtMaterial
