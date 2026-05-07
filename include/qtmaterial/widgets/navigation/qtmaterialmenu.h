#pragma once

#include <QIcon>
#include <QPoint>
#include <QString>
#include <QVector>
#include <QWidget>

#include "qtmaterial/qtmaterialglobal.h"

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialMenu : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QString accessibilitySummary READ accessibilitySummary NOTIFY accessibilitySummaryChanged)

public:
    enum class ItemRole {
        Action,
        Separator,
    };
    Q_ENUM(ItemRole)

    struct Item {
        QString text;
        QIcon icon;
        ItemRole role = ItemRole::Action;
        bool enabled = true;
        bool checkable = false;
        bool checked = false;
    };

    explicit QtMaterialMenu(QWidget* parent = nullptr);
    ~QtMaterialMenu() override;

    int addItem(const QString& text, const QIcon& icon = QIcon());
    int addSeparator();
    void clear();

    int count() const noexcept;
    bool isEmpty() const noexcept;

    QString itemText(int index) const;
    void setItemText(int index, const QString& text);

    QIcon itemIcon(int index) const;
    void setItemIcon(int index, const QIcon& icon);

    bool isSeparator(int index) const;
    bool isItemEnabled(int index) const;
    void setItemEnabled(int index, bool enabled);

    bool isItemCheckable(int index) const;
    void setItemCheckable(int index, bool checkable);

    bool isItemChecked(int index) const;
    void setItemChecked(int index, bool checked);

    int currentIndex() const noexcept;
    void setCurrentIndex(int index);

    int itemAt(const QPoint& position) const;
    QRect itemRect(int index) const;

    QString itemAccessibleText(int index) const;
    QString accessibilitySummary() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

Q_SIGNALS:
    void activated(int index);
    void currentIndexChanged(int index);
    void accessibilitySummaryChanged(const QString& summary);
    void dismissed();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;

private:
    bool isValidIndex(int index) const noexcept;
    bool isActivatableIndex(int index) const;
    int firstActivatableIndex() const;
    int lastActivatableIndex() const;
    int nextActivatableIndex(int start, int step) const;
    void activateIndex(int index);
    void ensureCurrentIndex();
    void updateAccessibility();
    void emitAccessibilitySummaryIfChanged();

    QVector<Item> m_items;
    int m_currentIndex = -1;
    int m_pressedIndex = -1;
    QString m_lastAccessibilitySummary;
};
