#pragma once

#include <memory>

#include <QWidget>
#include <qicon.h>

#include "qtmaterial/core/qtmaterialcontrol.h"
#include "qtmaterial/qtmaterialglobal.h"

class QKeyEvent;
class QMouseEvent;
class QPaintEvent;
class QResizeEvent;

namespace QtMaterial {

class QtMaterialListItemPrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialListItem : public QtMaterialControl
{
    Q_OBJECT
public:
    enum class DensityVariant {
        Compact,
        Standard,
        Large
    };

    explicit QtMaterialListItem(QWidget* parent = nullptr);
    explicit QtMaterialListItem(const QString& headline, QWidget* parent = nullptr);
    ~QtMaterialListItem() override;

    QString headlineText() const;
    void setHeadlineText(const QString& text);

    QString supportingText() const;
    void setSupportingText(const QString& text);

    QIcon leadingIcon() const;
    void setLeadingIcon(const QIcon& icon);

    QIcon trailingIcon() const;
    void setTrailingIcon(const QIcon& icon);

    bool isSelected() const noexcept;
    void setSelected(bool selected);

    bool isDividerVisible() const noexcept;
    void setDividerVisible(bool visible);

    DensityVariant densityVariant() const noexcept;
    void setDensityVariant(DensityVariant variant);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void clicked();
    void activated();
    void selectionChanged(bool selected);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void changeEvent(QEvent* event) override;

    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void stateChangedEvent() override;
    void contentChangedEvent();

private:
    std::unique_ptr<QtMaterialListItemPrivate> d_ptr;
    void ensureSpecResolved() const;
    void ensureLayoutResolved() const;
    void invalidateLayoutCache();

};

} // namespace QtMaterial
