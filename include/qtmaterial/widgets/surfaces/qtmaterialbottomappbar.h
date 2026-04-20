#pragma once

#include <QIcon>
#include <QPointer>
#include <QString>
#include <QVector>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/core/qtmaterialsurface.h"

class QAbstractButton;
class QToolButton;

namespace QtMaterial {
struct BottomAppBarSpec;
}

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialBottomAppBar : public QtMaterial::QtMaterialSurface
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(bool elevated READ elevated WRITE setElevated NOTIFY elevatedChanged)
    Q_PROPERTY(bool fabAttached READ fabAttached WRITE setFabAttached NOTIFY fabAttachedChanged)

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

    int actionCount() const noexcept;
    int addActionButton(const QIcon& icon, const QString& toolTip = QString());
    void clearActionButtons();

    void setFabButton(QAbstractButton* button);
    QAbstractButton* fabButton() const noexcept;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void titleChanged(const QString& title);
    void elevatedChanged(bool value);
    void fabAttachedChanged(bool value);
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
    struct ActionButtonEntry {
        QPointer<QToolButton> button;
        int index = -1;
    };

    void ensureSpecResolved() const;
    void ensureLayoutResolved() const;
    void invalidateSpecCache();
    void invalidateLayoutCache();
    void syncButtons();
    void syncAccessibility();
    QRect availableContentRect() const;

    QString m_title;
    QIcon m_navigationIcon;
    bool m_elevated = false;
    bool m_fabAttached = false;

    mutable bool m_specDirty = true;
    mutable bool m_layoutDirty = true;
    // mutable QtMaterial::BottomAppBarSpec m_spec;
    mutable QRect m_visualRect;
    mutable QRect m_navRect;
    mutable QRect m_titleRect;
    mutable QRect m_fabRect;
    mutable QVector<QRect> m_actionRects;
    mutable QPainterPath m_containerPath;

    QPointer<QToolButton> m_navigationButton;
    QVector<ActionButtonEntry> m_actionButtons;
    QPointer<QAbstractButton> m_fabButton;
};
