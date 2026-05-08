#pragma once

#include <memory>

#include <QIcon>
#include <QSize>
#include <QString>
#include <QVector>

#include "qtmaterial/core/qtmaterialcontrol.h"
#include "qtmaterial/specs/qtmaterialnavigationrailspec.h"
#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

class QtMaterialNavigationRailPrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialNavigationRail : public QtMaterialControl {
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(bool labelsVisible READ labelsVisible WRITE setLabelsVisible NOTIFY labelsVisibleChanged)
    Q_PROPERTY(QString accessibilitySummary READ accessibilitySummary NOTIFY accessibilitySummaryChanged)

public:
    explicit QtMaterialNavigationRail(QWidget* parent = nullptr);
    ~QtMaterialNavigationRail() override;

    int addDestination(const QString& text, const QIcon& icon = QIcon());
    void insertDestination(int index, const QString& text, const QIcon& icon = QIcon());
    void removeDestination(int index);
    void clearDestinations();

    int count() const noexcept;
    QString destinationText(int index) const;
    QIcon destinationIcon(int index) const;

    bool isDestinationEnabled(int index) const noexcept;
    void setDestinationEnabled(int index, bool enabled);

    QString destinationAccessibleText(int index) const;
    QString accessibilitySummary() const;

    int currentIndex() const noexcept;
    void setCurrentIndex(int index);

    bool labelsVisible() const noexcept;
    void setLabelsVisible(bool visible);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void currentIndexChanged(int index);
    void destinationActivated(int index);
    void labelsVisibleChanged(bool visible);
    void destinationEnabledChanged(int index, bool enabled);
    void accessibilitySummaryChanged(const QString& summary);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void invalidateResolvedSpec() override;

private:
    std::unique_ptr<QtMaterialNavigationRailPrivate> d_ptr;

    void ensureSpecResolved() const;
    QRect itemRect(int index) const;
    QRect indicatorRect(const QRect& itemRect) const;
    int indexAt(const QPoint& pos) const;

    int firstEnabledIndex() const noexcept;
    int lastEnabledIndex() const noexcept;
    int nextEnabledIndex(int from, int step) const noexcept;
    void syncAccessibility();


};

} // namespace QtMaterial
