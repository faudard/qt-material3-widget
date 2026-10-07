#pragma once

#include <QByteArray>
#include <QIcon>
#include <QString>

#include <memory>

#include "qtmaterial/core/qtmaterialcontrol.h"
#include "qtmaterial/foundation/qtmaterialwindowsizeclass.h"
#include "qtmaterial/qtmaterialglobal.h"

class QEvent;
class QFocusEvent;
class QKeyEvent;
class QMouseEvent;
class QPaintEvent;

namespace QtMaterial {

class QtMaterialNavigationSuitePrivate;

enum class NavigationSuiteType
{
    NavigationBar,
    NavigationRail
};

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialNavigationSuite : public QtMaterialControl
{
    Q_OBJECT

public:
    explicit QtMaterialNavigationSuite(QWidget* parent = nullptr);
    ~QtMaterialNavigationSuite() override;

    int addDestination(const QString& text, const QIcon& icon = QIcon());
    void insertDestination(int index, const QString& text, const QIcon& icon = QIcon());
    void removeDestination(int index);
    void clearDestinations();

    int count() const noexcept;
    QString destinationText(int index) const;
    QIcon destinationIcon(int index) const;
    bool isDestinationEnabled(int index) const noexcept;
    void setDestinationEnabled(int index, bool enabled);

    int currentIndex() const noexcept;
    void setCurrentIndex(int index);

    WindowWidthSizeClass windowWidthSizeClass() const noexcept;
    void setWindowWidthSizeClass(WindowWidthSizeClass sizeClass);

    NavigationSuiteType navigationType() const noexcept;

    QString destinationAccessibleText(int index) const;
    QString accessibilitySummary() const;

    /**
     * Saves selected destination, adaptive width class and per-destination
     * enabled state. Destination text/order is embedded as a compatibility
     * signature; icons are deliberately not serialized.
     */
    QByteArray saveWorkspaceState() const;

    /**
     * Restores state only when the current destination text/order matches the
     * saved navigation model. Incompatible payloads are rejected unchanged.
     */
    bool restoreWorkspaceState(const QByteArray& state);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void currentIndexChanged(int index);
    void destinationActivated(int index);
    void destinationEnabledChanged(int index, bool enabled);
    void windowWidthSizeClassChanged(QtMaterial::WindowWidthSizeClass sizeClass);
    void navigationTypeChanged(QtMaterial::NavigationSuiteType type);
    void accessibilitySummaryChanged(const QString& summary);

protected:
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void invalidateResolvedSpec() override;

private:
    void ensureSpecResolved() const;
    void syncAccessibility();

    std::unique_ptr<QtMaterialNavigationSuitePrivate> d_ptr;
};

} // namespace QtMaterial
