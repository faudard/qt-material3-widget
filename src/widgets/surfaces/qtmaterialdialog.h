#pragma once

#include <QPointer>
#include <QPainterPath>
#include <QRect>

#include "qtmaterial/widgets/surfaces/qtmaterialdialog.h" // keep original public include path in consumer tree
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/effects/qtmaterialscrimwidget.h"
#include "qtmaterial/specs/qtmaterialdialogspec.h"

class QKeyEvent;
class QResizeEvent;
class QShowEvent;
class QHideEvent;

class QtMaterialDialog : public QtMaterial::QtMaterialOverlaySurface
{
    Q_OBJECT
public:
    explicit QtMaterialDialog(QWidget* parent = nullptr);
    ~QtMaterialDialog() override;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void open();
    void close();

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

    mutable bool m_specDirty = true;
    mutable bool m_layoutDirty = true;
    mutable QtMaterial::DialogSpec m_spec;
    mutable QRect m_cachedVisualRect;
    mutable QRect m_cachedContentRect;
    mutable QPainterPath m_cachedContainerPath;
    mutable qreal m_cachedCornerRadius = 0.0;
    mutable int m_cachedShadowMargin = 0;

    QPointer<QtMaterial::QtMaterialScrimWidget> m_scrim;
    QtMaterial::QtMaterialTransitionController* m_transition = nullptr;
};
