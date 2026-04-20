#pragma once

#include <QPainterPath>
#include <QRect>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/core/qtmaterialabstractbutton.h"
#include "qtmaterial/specs/qtmaterialiconbuttonspec.h"

namespace QtMaterial {

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialIconButton : public QtMaterialAbstractButton
{
    Q_OBJECT
public:
    explicit QtMaterialIconButton(QWidget* parent = nullptr);
    explicit QtMaterialIconButton(const QIcon& icon, QWidget* parent = nullptr);
    ~QtMaterialIconButton() override;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    bool isSelected() const noexcept;
    void setSelected(bool selected);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void changeEvent(QEvent* event) override;
    void themeChangedEvent(const Theme& theme) override;
    void stateChangedEvent() override;
    void contentChangedEvent() override;

    virtual IconButtonSpec resolveIconButtonSpec() const;

private:
    void invalidateLayoutCache();
    void invalidateResolvedSpec();
    void ensureSpecResolved() const;
    void ensureLayoutResolved() const;

    QRect visualContainerRect() const;
    QPainterPath containerPath() const;
    QRect iconRect() const;

    mutable bool m_specDirty = true;
    mutable bool m_layoutDirty = true;
    mutable IconButtonSpec m_spec;
    mutable QRect m_cachedVisualRect;
    mutable QRect m_cachedIconRect;
    mutable qreal m_cachedCornerRadius = 0.0;
    mutable QPainterPath m_cachedContainerPath;

    bool m_selected = false;
};

} // namespace QtMaterial
