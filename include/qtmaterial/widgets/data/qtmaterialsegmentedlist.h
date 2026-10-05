#pragma once

#include "qtmaterial/widgets/data/qtmateriallist.h"

namespace QtMaterial {

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialSegmentedList : public QtMaterialList
{
    Q_OBJECT
    Q_PROPERTY(bool expressive READ expressive WRITE setExpressive NOTIFY expressiveChanged)

public:
    explicit QtMaterialSegmentedList(QWidget* parent = nullptr);
    ~QtMaterialSegmentedList() override;

    bool expressive() const noexcept;
    void setExpressive(bool expressive);

    void addItem(QtMaterialListItem* item);
    QtMaterialListItem* addItem(const QString& headline);
    void insertItem(int index, QtMaterialListItem* item);
    QtMaterialListItem* takeItem(int index);
    void removeItem(int index);
    void removeItem(QtMaterialListItem* item);
    void clear();

Q_SIGNALS:
    void expressiveChanged(bool expressive);

private:
    void refreshSegments();

    bool m_expressive = true;
};

} // namespace QtMaterial
