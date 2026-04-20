#pragma once

#include <QPainterPath>
#include <QPointer>
#include <QRect>

#include "qtmaterial/core/qtmaterialoverlaysurface.h"
#include "qtmaterial/specs/qtmaterialdialogspec.h"
#include "qtmaterial/qtmaterialglobal.h"

class QEvent;
class QHideEvent;
class QKeyEvent;
class QPaintEvent;
class QResizeEvent;
class QShowEvent;

namespace QtMaterial {

class QtMaterialScrimWidget;
class QtMaterialTransitionController;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialDialog : public QtMaterialOverlaySurface
{
    Q_OBJECT

public:
    explicit QtMaterialDialog(QWidget* parent = nullptr);
    ~QtMaterialDialog() override;

    void setBodyWidget(QWidget* widget);
    QWidget* bodyWidget() const;

    void open();
    void close();

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    void changeEvent(QEvent* event) override;

    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void invalidateResolvedSpec() override;

private:
    void resolveSpecIfNeeded() const;
    void ensureLayoutResolved() const;
    void invalidateLayoutCache();

    void syncChildGeometry() const;
    void syncAccessibilityState();
    void updateScrimForProgress(qreal progress);
    void focusInitialChild();
    QWidget* firstFocusableChild() const;

    static qreal cornerRadiusForShape(ShapeRole role);

private:
    mutable bool m_specDirty = true;
    mutable bool m_layoutDirty = true;
    mutable DialogSpec m_spec;

    bool m_closing = false;

    QPointer<QWidget> m_bodyWidget;
    QtMaterialScrimWidget* m_scrim = nullptr;
    QtMaterialTransitionController* m_transition = nullptr;

    mutable QRect m_cachedVisualRect;
    mutable QRect m_cachedContentRect;
    mutable QPainterPath m_cachedContainerPath;
    mutable qreal m_cachedCornerRadius = 0.0;
    mutable int m_cachedShadowMargin = 12;
};

} // namespace QtMaterial