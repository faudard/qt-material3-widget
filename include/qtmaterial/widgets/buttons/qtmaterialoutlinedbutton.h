#pragma once

class QEvent;
class QPaintEvent;
class QResizeEvent;

#include <QPainterPath>
#include <QRect>
#include <QString>

#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"

namespace QtMaterial {
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialOutlinedButton : public QtMaterialTextButton
{
    Q_OBJECT
public:
    explicit QtMaterialOutlinedButton(QWidget* parent = nullptr);
    ~QtMaterialOutlinedButton() override;

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void changeEvent(QEvent* event) override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void invalidateResolvedSpec() override;
    void contentChangedEvent() override;
    ButtonSpec resolveButtonSpec() const override;

private:
    void invalidateLayoutCache();
    void ensureLayoutResolved() const;

    mutable bool m_layoutDirty = true;
    mutable QRect m_cachedVisualRect;
    mutable QPainterPath m_cachedContainerPath;
    mutable qreal m_cachedCornerRadius = 0.0;
    mutable QRect m_cachedIconRect;
    mutable QRect m_cachedTextRect;
    mutable QString m_cachedElidedText;
    mutable bool m_cachedHasIcon = false;
};
} // namespace QtMaterial
