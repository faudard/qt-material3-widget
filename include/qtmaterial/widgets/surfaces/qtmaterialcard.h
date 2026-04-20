#pragma once

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/core/qtmaterialsurface.h"
#include "qtmaterial/specs/qtmaterialcardspec.h"

class QPaintEvent;
class QResizeEvent;
class QEvent;

namespace QtMaterial {

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialCard : public QtMaterialSurface
{
    Q_OBJECT
public:
    explicit QtMaterialCard(QWidget* parent = nullptr);
    ~QtMaterialCard() override;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void setTitleText(const QString& text);
    QString titleText() const;

    void setBodyText(const QString& text);
    QString bodyText() const;

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void changeEvent(QEvent* event) override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void stateChangedEvent() override;

private:
    void ensureSpecResolved() const;
    void ensureLayoutResolved() const;
    void invalidateLayoutCache();
    QRect visualRect() const;
    QRect contentRectForPaint() const;
    QPainterPath containerPath() const;

private:
    QString m_titleText;
    QString m_bodyText;

    mutable bool m_specDirty = true;
    mutable bool m_layoutDirty = true;
    mutable QtMaterial::CardSpec m_spec;
    mutable QRect m_cachedVisualRect;
    mutable QRect m_cachedContentRect;
    mutable qreal m_cachedCornerRadius = 0.0;
    mutable QPainterPath m_cachedContainerPath;
};

} // namespace QtMaterial
