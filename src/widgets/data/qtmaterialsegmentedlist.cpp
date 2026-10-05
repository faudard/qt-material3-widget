#include "qtmaterial/widgets/data/qtmaterialsegmentedlist.h"

#include "qtmaterial/widgets/data/qtmateriallistitem.h"

namespace QtMaterial {

QtMaterialSegmentedList::QtMaterialSegmentedList(QWidget* parent)
    : QtMaterialList(parent)
{
    setMaterialComponent(QStringLiteral("SegmentedList"));
    setDividersVisible(false);
    setAccessibleName(tr("Segmented list"));
}

QtMaterialSegmentedList::~QtMaterialSegmentedList() = default;

bool QtMaterialSegmentedList::expressive() const noexcept
{
    return m_expressive;
}

void QtMaterialSegmentedList::setExpressive(bool expressive)
{
    if (m_expressive == expressive) {
        return;
    }

    m_expressive = expressive;
    refreshSegments();
    Q_EMIT expressiveChanged(expressive);
}

void QtMaterialSegmentedList::addItem(QtMaterialListItem* item)
{
    QtMaterialList::addItem(item);
    refreshSegments();
}

QtMaterialListItem* QtMaterialSegmentedList::addItem(const QString& headline)
{
    QtMaterialListItem* item = QtMaterialList::addItem(headline);
    refreshSegments();
    return item;
}

void QtMaterialSegmentedList::insertItem(int index, QtMaterialListItem* item)
{
    QtMaterialList::insertItem(index, item);
    refreshSegments();
}

QtMaterialListItem* QtMaterialSegmentedList::takeItem(int index)
{
    QtMaterialListItem* item = QtMaterialList::takeItem(index);
    if (item) {
        item->setExpressive(false);
        item->setExpressiveSegmentPosition(
            QtMaterialListItem::ExpressiveSegmentPosition::None);
    }
    refreshSegments();
    return item;
}

void QtMaterialSegmentedList::removeItem(int index)
{
    QtMaterialList::removeItem(index);
    refreshSegments();
}

void QtMaterialSegmentedList::removeItem(QtMaterialListItem* item)
{
    QtMaterialList::removeItem(item);
    refreshSegments();
}

void QtMaterialSegmentedList::clear()
{
    QtMaterialList::clear();
    refreshSegments();
}

void QtMaterialSegmentedList::refreshSegments()
{
    const int itemCount = count();
    for (int index = 0; index < itemCount; ++index) {
        QtMaterialListItem* item = itemAt(index);
        if (!item) {
            continue;
        }

        item->setExpressive(m_expressive);

        QtMaterialListItem::ExpressiveSegmentPosition position =
            QtMaterialListItem::ExpressiveSegmentPosition::Middle;

        if (itemCount == 1) {
            position = QtMaterialListItem::ExpressiveSegmentPosition::Single;
        } else if (index == 0) {
            position = QtMaterialListItem::ExpressiveSegmentPosition::First;
        } else if (index == itemCount - 1) {
            position = QtMaterialListItem::ExpressiveSegmentPosition::Last;
        }

        item->setExpressiveSegmentPosition(position);
    }

    setAccessibleDescription(
        tr("%1 items").arg(itemCount));
}

} // namespace QtMaterial
