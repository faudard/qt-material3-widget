#include "qtmaterial/widgets/data/qtmaterialsegmentedlist.h"

#include "qtmaterial/widgets/data/qtmateriallistitem.h"

namespace QtMaterial {

QtMaterialSegmentedList::QtMaterialSegmentedList(QWidget* parent)
    : QtMaterialList(parent)
{
    setMaterialComponent(QStringLiteral("SegmentedList"));
    setDividersVisible(false);
    setAccessibleName(tr("Segmented list"));

    connect(this, &QtMaterialList::countChanged,
            this, [this](int) { refreshSegments(); });
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
    Q_EMIT itemLabelsChanged(itemLabels());
}

QtMaterialListItem* QtMaterialSegmentedList::addItem(const QString& headline)
{
    QtMaterialListItem* item = QtMaterialList::addItem(headline);
    refreshSegments();
    Q_EMIT itemLabelsChanged(itemLabels());
    return item;
}

void QtMaterialSegmentedList::insertItem(int index, QtMaterialListItem* item)
{
    QtMaterialList::insertItem(index, item);
    refreshSegments();
    Q_EMIT itemLabelsChanged(itemLabels());
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
    if (item) {
        Q_EMIT itemLabelsChanged(itemLabels());
    }
    return item;
}

void QtMaterialSegmentedList::removeItem(int index)
{
    if (index < 0 || index >= count()) {
        return;
    }
    QtMaterialList::removeItem(index);
    refreshSegments();
    Q_EMIT itemLabelsChanged(itemLabels());
}

void QtMaterialSegmentedList::removeItem(QtMaterialListItem* item)
{
    if (!item || indexOf(item) < 0) {
        return;
    }
    QtMaterialList::removeItem(item);
    refreshSegments();
    Q_EMIT itemLabelsChanged(itemLabels());
}

void QtMaterialSegmentedList::clear()
{
    const bool hadItems = count() > 0;
    QtMaterialList::clear();
    refreshSegments();
    if (hadItems) {
        Q_EMIT itemLabelsChanged(itemLabels());
    }
}

QStringList QtMaterialSegmentedList::itemLabels() const
{
    QStringList labels;
    labels.reserve(count());
    for (int index = 0; index < count(); ++index) {
        if (const QtMaterialListItem* item = itemAt(index)) {
            labels.append(item->headlineText());
        }
    }
    return labels;
}

void QtMaterialSegmentedList::setItemLabels(const QStringList& labels)
{
    if (itemLabels() == labels) {
        return;
    }

    while (count() > 0) {
        removeItem(0);
    }
    for (const QString& label : labels) {
        addItem(label);
    }
    Q_EMIT itemLabelsChanged(itemLabels());
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
