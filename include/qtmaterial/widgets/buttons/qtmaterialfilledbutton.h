#pragma once

class QEvent;
class QPaintEvent;
class QPainter;
class QResizeEvent;

#include <QPainterPath>
#include <QRect>
#include <QString>

#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"

namespace QtMaterial {

class QtMaterialTransitionController;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialFilledButton : public QtMaterialTextButton {
    Q_OBJECT
public:
    explicit QtMaterialFilledButton(QWidget* parent = nullptr);
    ~QtMaterialFilledButton() override;

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void changeEvent(QEvent* event) override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void invalidateResolvedSpec() override;
    void contentChangedEvent() override;
    void stateChangedEvent() override;
    ButtonSpec resolveButtonSpec() const override;

private:
    void invalidateLayoutCache();
    void ensureLayoutResolved() const;
    void syncElevationAnimation();
    qreal targetElevationProgress() const noexcept;
    qreal animatedElevationProgress() const noexcept;
    void paintInteractionElevation(QPainter* painter) const;

    mutable bool m_layoutDirty = true;
    mutable QRect m_cachedVisualRect;
    mutable QPainterPath m_cachedContainerPath;
    mutable qreal m_cachedCornerRadius = 0.0;
    mutable QRect m_cachedIconRect;
    mutable QRect m_cachedTextRect;
    mutable QString m_cachedElidedText;
    mutable bool m_cachedHasIcon = false;
    QtMaterialTransitionController* m_elevationTransition = nullptr;
};

} // namespace QtMaterial
