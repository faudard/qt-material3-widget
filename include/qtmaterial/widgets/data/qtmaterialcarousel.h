#pragma once

#include <memory>

#include <QIcon>
#include <QPoint>
#include <QSize>
#include <QString>
#include <QVector>

#include "qtmaterial/core/qtmaterialcontrol.h"
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialdatacomponentspecs.h"

class QEvent;
class QKeyEvent;
class QMouseEvent;
class QPaintEvent;

namespace QtMaterial {

class QtMaterialCarouselPrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialCarousel
    : public QtMaterialControl
{
    Q_OBJECT

    Q_PROPERTY(
        int currentIndex
        READ currentIndex
        WRITE setCurrentIndex
        NOTIFY currentIndexChanged)
    Q_PROPERTY(
        bool wrapAround
        READ wrapAround
        WRITE setWrapAround
        NOTIFY wrapAroundChanged)
    Q_PROPERTY(
        int visibleItemCount
        READ visibleItemCount
        WRITE setVisibleItemCount
        NOTIFY visibleItemCountChanged)
    Q_PROPERTY(
        QString accessibilitySummary
        READ accessibilitySummary
        NOTIFY accessibilitySummaryChanged)

public:
    struct Item
    {
        QString title;
        QString supportingText;
        QIcon icon;
        bool enabled = true;
    };

    explicit QtMaterialCarousel(
        QWidget* parent = nullptr);
    ~QtMaterialCarousel() override;

    int addItem(
        const QString& title,
        const QString& supportingText = QString(),
        const QIcon& icon = QIcon());
    void insertItem(
        int index,
        const QString& title,
        const QString& supportingText = QString(),
        const QIcon& icon = QIcon());
    void removeItem(int index);
    void clear();

    int count() const noexcept;
    bool isEmpty() const noexcept;

    QString itemTitle(int index) const;
    void setItemTitle(
        int index,
        const QString& title);

    QString itemSupportingText(int index) const;
    void setItemSupportingText(
        int index,
        const QString& text);

    QIcon itemIcon(int index) const;
    void setItemIcon(
        int index,
        const QIcon& icon);

    bool isItemEnabled(int index) const;
    void setItemEnabled(
        int index,
        bool enabled);

    int currentIndex() const noexcept;
    void setCurrentIndex(int index);

    bool wrapAround() const noexcept;
    void setWrapAround(bool wrap);

    int visibleItemCount() const noexcept;
    void setVisibleItemCount(int count);

    QString itemAccessibleText(int index) const;
    QString currentItemAccessibleText() const;
    QString accessibilitySummary() const;

    const CarouselSpec& resolvedSpec() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void next();
    void previous();
    void activateCurrentItem();

Q_SIGNALS:
    void currentIndexChanged(int index);
    void itemActivated(int index);
    void itemClicked(int index);
    void itemCountChanged(int count);
    void wrapAroundChanged(bool wrapAround);
    void visibleItemCountChanged(int count);
    void accessibilitySummaryChanged(
        const QString& summary);

protected:
    void invalidateResolvedSpec() override;
    void stateChangedEvent() override;
    void themeChangedEvent(
        const QtMaterial::Theme& theme) override;

    void keyPressEvent(QKeyEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void changeEvent(QEvent* event) override;

private:
    void ensureSpecResolved() const;
    int indexAtPosition(const QPoint& position) const;

    std::unique_ptr<QtMaterialCarouselPrivate> d_ptr;
};

} // namespace QtMaterial
