#pragma once

#include <QIcon>
#include <QVector>

#include "qtmaterial/core/qtmaterialwidget.h"
#include "qtmaterial/qtmaterialglobal.h"

class QBoxLayout;
class QEvent;
class QPaintEvent;

namespace QtMaterial {

class QtMaterialIconButton;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialFloatingToolbar : public QtMaterialWidget
{
    Q_OBJECT
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
    Q_PROPERTY(bool expanded READ isExpanded WRITE setExpanded NOTIFY expandedChanged)
    Q_PROPERTY(int spacing READ spacing WRITE setSpacing NOTIFY spacingChanged)

public:
    explicit QtMaterialFloatingToolbar(QWidget* parent = nullptr);
    ~QtMaterialFloatingToolbar() override;

    Qt::Orientation orientation() const noexcept;
    void setOrientation(Qt::Orientation orientation);

    bool isExpanded() const noexcept;
    void setExpanded(bool expanded);

    int spacing() const noexcept;
    void setSpacing(int spacing);

    int count() const noexcept;
    QWidget* itemAt(int index) const;

    QtMaterialIconButton* addAction(
        const QIcon& icon,
        const QString& accessibleName);
    void addWidget(QWidget* widget);
    void removeWidget(QWidget* widget);
    void clear();

Q_SIGNALS:
    void actionTriggered(int index);
    void orientationChanged(Qt::Orientation orientation);
    void expandedChanged(bool expanded);
    void spacingChanged(int spacing);

protected:
    void paintEvent(QPaintEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;

private:
    int itemIndex(const QObject* object) const noexcept;
    int nextFocusable(int start, int step) const noexcept;
    void syncVisibility();
    void focusIndex(int index);

    QBoxLayout* m_layout = nullptr;
    QVector<QWidget*> m_items;
    Qt::Orientation m_orientation = Qt::Horizontal;
    bool m_expanded = true;
};

} // namespace QtMaterial
