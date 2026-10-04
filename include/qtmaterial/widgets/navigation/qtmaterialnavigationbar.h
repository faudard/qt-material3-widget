#pragma once

#include <QIcon>
#include <QString>
#include <memory>

#include "qtmaterial/core/qtmaterialcontrol.h"
#include "qtmaterial/qtmaterialglobal.h"

class QEvent;
class QKeyEvent;
class QMouseEvent;
class QPaintEvent;

namespace QtMaterial {

class QtMaterialNavigationBarPrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialNavigationBar : public QtMaterialControl
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(bool labelsVisible READ labelsVisible WRITE setLabelsVisible NOTIFY labelsVisibleChanged)
    Q_PROPERTY(QString accessibilitySummary READ accessibilitySummary NOTIFY accessibilitySummaryChanged)

public:
    explicit QtMaterialNavigationBar(QWidget* parent = nullptr);
    ~QtMaterialNavigationBar() override;

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

    int currentIndex() const noexcept;
    void setCurrentIndex(int index);

    bool labelsVisible() const noexcept;
    void setLabelsVisible(bool visible);

    QString accessibilitySummary() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

Q_SIGNALS:
    void currentIndexChanged(int index);
    void destinationActivated(int index);
    void destinationEnabledChanged(int index, bool enabled);
    void labelsVisibleChanged(bool visible);
    void accessibilitySummaryChanged(const QString& summary);

protected:
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void changeEvent(QEvent* event) override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;

private:
    void syncAccessibility();
    std::unique_ptr<QtMaterialNavigationBarPrivate> d_ptr;
};

} // namespace QtMaterial
