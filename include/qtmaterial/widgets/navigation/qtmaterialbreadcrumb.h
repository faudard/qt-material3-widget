#pragma once

#include <QStringList>
#include <QWidget>

#include "qtmaterial/qtmaterialglobal.h"

class QHBoxLayout;

namespace QtMaterial {

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialBreadcrumb : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)

public:
    explicit QtMaterialBreadcrumb(QWidget* parent = nullptr);

    QStringList items() const;
    void setItems(const QStringList& items);
    void addItem(const QString& text);
    void clear();

    int currentIndex() const noexcept;
    void setCurrentIndex(int index);

Q_SIGNALS:
    void activated(int index, const QString& text);
    void currentIndexChanged(int index);

private:
    void rebuild();

    QStringList m_items;
    int m_currentIndex = -1;
    QHBoxLayout* m_layout = nullptr;
};

} // namespace QtMaterial
