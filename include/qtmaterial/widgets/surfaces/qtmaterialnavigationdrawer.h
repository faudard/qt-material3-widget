#pragma once

#include <QPointer>
#include <QPainterPath>
#include <QRect>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/core/qtmaterialoverlaysurface.h"
#include "qtmaterial/specs/qtmaterialnavigationdrawerspec.h"

class QKeyEvent;
class QResizeEvent;
class QShowEvent;
class QHideEvent;
namespace QtMaterial {

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
    void ensureLayoutResolved() const;
    void invalidateLayoutCache();
    void updateScrimVisuals();
    void focusInitialChild();
    QWidget* firstFocusableChild() const;

private:
    Edge m_edge = Edge::Left;
    bool m_open = false;

    mutable bool m_specDirty = true;
    mutable bool m_layoutDirty = true;
    mutable QtMaterial::NavigationDrawerSpec m_spec;
    mutable QRect m_cachedVisualRect;
    mutable QRect m_cachedContentRect;
    mutable QPainterPath m_cachedContainerPath;
    mutable qreal m_cachedCornerRadius = 0.0;
    mutable int m_cachedShadowMargin = 0;

    QPointer<QtMaterialScrimWidget> m_scrim;
    QtMaterialTransitionController* m_transition = nullptr;
};
}