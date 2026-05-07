#pragma once

#include <QIcon>
#include <QSize>
#include <QString>
#include <QVector>
#include <QWidget>

#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialCarousel : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(bool wrapAround READ wrapAround WRITE setWrapAround NOTIFY wrapAroundChanged)
    Q_PROPERTY(int visibleItemCount READ visibleItemCount WRITE setVisibleItemCount NOTIFY visibleItemCountChanged)
    Q_PROPERTY(QString accessibilitySummary READ accessibilitySummary NOTIFY accessibilitySummaryChanged)

public:
    struct Item {
        QString title;
        QString supportingText;
        QIcon icon;
        bool enabled = true;
    };

    explicit QtMaterialCarousel(QWidget* parent = nullptr);
    ~QtMaterialCarousel() override;

    int addItem(const QString& title,
                const QString& supportingText = QString(),
                const QIcon& icon = QIcon());
    void insertItem(int index,
                    const QString& title,
                    const QString& supportingText = QString(),
                    const QIcon& icon = QIcon());
    void removeItem(int index);
    void clear();

    int count() const noexcept;
    bool isEmpty() const noexcept;

    QString itemTitle(int index) const;
    void setItemTitle(int index, const QString& title);

    QString itemSupportingText(int index) const;
    void setItemSupportingText(int index, const QString& text);

    QIcon itemIcon(int index) const;
    void setItemIcon(int index, const QIcon& icon);

    bool isItemEnabled(int index) const;
    void setItemEnabled(int index, bool enabled);

    int currentIndex() const noexcept;
    void setCurrentIndex(int index);

    bool wrapAround() const noexcept;
    void setWrapAround(bool wrap);

    int visibleItemCount() const noexcept;
    void setVisibleItemCount(int count);

    QString itemAccessibleText(int index) const;
    QString currentItemAccessibleText() const;
    QString accessibilitySummary() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public slots:
    void next();
    void previous();
    void activateCurrentItem();

signals:
    void currentIndexChanged(int index);
    void itemActivated(int index);
    void itemClicked(int index);
    void itemCountChanged(int count);
    void wrapAroundChanged(bool wrapAround);
    void visibleItemCountChanged(int count);
    void accessibilitySummaryChanged(const QString& summary);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void changeEvent(QEvent* event) override;

private:
    bool isValidIndex(int index) const noexcept;
    int firstVisibleIndex() const noexcept;
    int nextEnabledIndex(int start, int delta) const noexcept;
    QRect itemRect(int index) const;
    void updateAccessibilitySummary();
    void emitCurrentIndexChanged(int index);

    QVector<Item> m_items;
    int m_currentIndex = -1;
    bool m_wrapAround = true;
    int m_visibleItemCount = 3;
    QString m_accessibilitySummary;
};

} // namespace QtMaterial
