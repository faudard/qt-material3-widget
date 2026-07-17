#pragma once

#include <memory>

#include <QPointer>
#include <QRect>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/core/qtmaterialoverlaysurface.h"

class QKeyEvent;
class QResizeEvent;
class QShowEvent;
class QHideEvent;
namespace QtMaterial {

class QtMaterialNavigationDrawerPrivate;
class QtMaterialScrimWidget;
class QtMaterialTransitionController;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialNavigationDrawer : public QtMaterial::QtMaterialOverlaySurface
{
    Q_OBJECT
public:
    enum class Edge {
        Left,
        Right
    };

    explicit QtMaterialNavigationDrawer(QWidget* parent = nullptr);
    ~QtMaterialNavigationDrawer() override;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void setEdge(Edge edge);
    Edge edge() const noexcept;

    void open();
    void closeDrawer();
    bool isOpen() const noexcept;

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    void changeEvent(QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void invalidateResolvedSpec() override;

private:
    void ensureSpecResolved() const;
    std::unique_ptr<QtMaterialNavigationDrawerPrivate> d_ptr;

};
}