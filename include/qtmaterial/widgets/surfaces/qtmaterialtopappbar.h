#pragma once

#include <QIcon>
#include <QPointer>
#include <QString>
#include <QVector>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/core/qtmaterialsurface.h"

class QToolButton;

namespace QtMaterial {
struct TopAppBarSpec;
}

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialTopAppBar : public QtMaterialSurface
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(bool centeredTitle READ centeredTitle WRITE setCenteredTitle NOTIFY centeredTitleChanged)
    Q_PROPERTY(bool elevated READ elevated WRITE setElevated NOTIFY elevatedChanged)

public:
    explicit QtMaterialTopAppBar(QWidget* parent = nullptr);
    explicit QtMaterialTopAppBar(const QString& title, QWidget* parent = nullptr);
    ~QtMaterialTopAppBar() override;

    QString title() const;
    void setTitle(const QString& title);

    bool centeredTitle() const noexcept;
    void setCenteredTitle(bool value);

    bool elevated() const noexcept;
    void setElevated(bool value);

    QIcon navigationIcon() const;
    void setNavigationIcon(const QIcon& icon);
    void clearNavigationIcon();

    int actionCount() const noexcept;
    int addActionButton(const QIcon& icon, const QString& toolTip = QString());
    void clearActionButtons();

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void titleChanged(const QString& title);
    void centeredTitleChanged(bool value);
    void elevatedChanged(bool value);
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
    bool m_centeredTitle = false;
    bool m_elevated = false;

    mutable bool m_specDirty = true;
    mutable bool m_layoutDirty = true;
    mutable QtMaterial::TopAppBarSpec* m_specCache = nullptr; // non-owning until copied below
    mutable QtMaterial::TopAppBarSpec m_spec;
    mutable QRect m_visualRect;
    mutable QRect m_navRect;
    mutable QRect m_titleRect;
    mutable QVector<QRect> m_actionRects;
    mutable QPainterPath m_containerPath;

    QPointer<QToolButton> m_navigationButton;
    QVector<ActionButtonEntry> m_actionButtons;
};
