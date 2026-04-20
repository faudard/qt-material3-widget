#pragma once

#include <QList>
#include <QWidget>

#include "qtmaterial/qtmaterialglobal.h"

class QVBoxLayout;

namespace QtMaterial {
struct ListSpec;
class Theme;
}

class QtMaterialListItem;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialList : public QWidget
{
    Q_OBJECT
public:
    explicit QtMaterialList(QWidget *parent = nullptr);
    ~QtMaterialList() override;

    int count() const noexcept;
    QtMaterialListItem *itemAt(int index) const noexcept;

    void addItem(QtMaterialListItem *item);
    void insertItem(int index, QtMaterialListItem *item);
    QtMaterialListItem *takeItem(int index);
    void clear();

    bool showDividers() const noexcept;
    void setShowDividers(bool on);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void itemActivated(int index);
    void currentIndexChanged(int index);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void ensureSpecResolved() const;
    void rebuildItemWiring();
    void updateCurrentIndex(int index);
    QRect viewportRectForPaint() const;
    QPainterPath viewportPathForPaint() const;

    mutable bool m_specDirty = true;
    mutable QtMaterial::ListSpec *m_cachedSpecPtr = nullptr; // patch-direction placeholder
    QList<QtMaterialListItem*> m_items;
    QVBoxLayout *m_layout = nullptr;
    int m_currentIndex = -1;
    bool m_showDividers = false;
};
