#pragma once

#include <QIcon>
#include <QPointer>
#include <QRect>
#include <QSize>
#include <QString>
#include <QVector>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/core/qtmaterialsurface.h"

#include <memory>
class QAbstractButton;
class QEvent;
class QKeyEvent;
class QPaintEvent;
class QResizeEvent;
class QToolButton;
class QtMaterialBottomAppBarPrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialBottomAppBar : public QtMaterial::QtMaterialSurface
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(bool elevated READ elevated WRITE setElevated NOTIFY elevatedChanged)
    Q_PROPERTY(bool fabAttached READ fabAttached WRITE setFabAttached NOTIFY fabAttachedChanged)
    Q_PROPERTY(QString navigationAccessibleName READ navigationAccessibleName WRITE setNavigationAccessibleName NOTIFY navigationAccessibleNameChanged)
    Q_PROPERTY(QString accessibilitySummary READ accessibilitySummary NOTIFY accessibilitySummaryChanged)

public:
    explicit QtMaterialBottomAppBar(QWidget* parent = nullptr);
    explicit QtMaterialBottomAppBar(const QString& title, QWidget* parent = nullptr);
    ~QtMaterialBottomAppBar() override;

    QString title() const;
    void setTitle(const QString& title);

    bool elevated() const noexcept;
    void setElevated(bool value);

    bool fabAttached() const noexcept;
    void setFabAttached(bool value);

    QIcon navigationIcon() const;
    void setNavigationIcon(const QIcon& icon);
    void clearNavigationIcon();

    QString navigationAccessibleName() const;
    void setNavigationAccessibleName(const QString& name);

    int actionCount() const noexcept;
    int addActionButton(const QIcon& icon, const QString& toolTip = QString());
    void clearActionButtons();

    QString actionAccessibleName(int index) const;
    void setActionAccessibleName(int index, const QString& name);
    QString actionAccessibleText(int index) const;

    void setFabButton(QAbstractButton* button);
    QAbstractButton* fabButton() const noexcept;
    QString fabAccessibleText() const;

    QString accessibilitySummary() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void titleChanged(const QString& title);
    void elevatedChanged(bool value);
    void fabAttachedChanged(bool value);
    void navigationAccessibleNameChanged(const QString& name);
    void accessibilitySummaryChanged(const QString& summary);
    void navigationTriggered();
    void actionTriggered(int index);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void changeEvent(QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void stateChangedEvent() override;

private:
    std::unique_ptr<QtMaterialBottomAppBarPrivate> d;

    const QtMaterial::AppBarSpec& resolvedSpec() const;
    void ensureSpecResolved() const;
    void ensureLayoutResolved() const;
    void invalidateLayoutCache();
    void syncButtons();
    void syncAccessibility();
    void emitAccessibilityIfChanged(const QString& previousSummary);
    QString effectiveNavigationAccessibleName() const;
    QRect availableContentRect() const;



};
