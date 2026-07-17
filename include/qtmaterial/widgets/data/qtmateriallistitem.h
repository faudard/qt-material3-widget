#pragma once

#include <QIcon>
#include <QSize>
#include <QString>
#include <Qt>

#include <memory>

#include "qtmaterial/core/qtmaterialcontrol.h"
#include "qtmaterial/specs/qtmateriallistitemspec.h"

class QEvent;
class QKeyEvent;
class QMouseEvent;
class QPaintEvent;

namespace QtMaterial {

class QtMaterialListItemPrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialListItem
    : public QtMaterialControl
{
    Q_OBJECT

public:
    enum class DensityVariant
    {
        Compact,
        Standard,
        Large
    };
    Q_ENUM(DensityVariant)

    Q_PROPERTY(
        QString headlineText
        READ headlineText
        WRITE setHeadlineText)
    Q_PROPERTY(
        QString supportingText
        READ supportingText
        WRITE setSupportingText)
    Q_PROPERTY(
        QIcon leadingIcon
        READ leadingIcon
        WRITE setLeadingIcon)
    Q_PROPERTY(
        QIcon trailingIcon
        READ trailingIcon
        WRITE setTrailingIcon)
    Q_PROPERTY(
        bool selected
        READ isSelected
        WRITE setSelected
        NOTIFY selectionChanged)
    Q_PROPERTY(
        bool dividerVisible
        READ isDividerVisible
        WRITE setDividerVisible)
    Q_PROPERTY(
        DensityVariant densityVariant
        READ densityVariant
        WRITE setDensityVariant)

    explicit QtMaterialListItem(
        QWidget* parent = nullptr);
    explicit QtMaterialListItem(
        const QString& headline,
        QWidget* parent = nullptr);
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

    const ListItemSpec& resolvedSpec() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

Q_SIGNALS:
    void clicked();
    void activated();
    void selectionChanged(bool selected);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void changeEvent(QEvent* event) override;

    void themeChangedEvent(
        const QtMaterial::Theme& theme) override;
    void invalidateResolvedSpec() override;
    void stateChangedEvent() override;

private:
    void ensureSpecResolved() const;
    int resolvedHeight() const;
    void contentChangedEvent();

    std::unique_ptr<QtMaterialListItemPrivate> d_ptr;
};

} // namespace QtMaterial
