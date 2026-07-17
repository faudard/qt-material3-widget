#include "qtmaterial/widgets/data/qtmateriallist.h"

#include <QPainter>
#include <QPainterPath>
#include <QPointer>
#include <QVBoxLayout>

#include "qtmaterial/specs/qtmaterialdataspecresolver.h"
#include "qtmaterial/widgets/data/qtmateriallistitem.h"

namespace QtMaterial {

struct QtMaterialListPrivate
{
    QVBoxLayout* layout = nullptr;
    QList<QPointer<QtMaterialListItem>> items;

    mutable ListSpec spec;
    mutable bool specDirty = true;

    bool dividersVisible = false;
    bool dividersExplicit = false;
};

QtMaterialList::QtMaterialList(QWidget* parent)
    : QtMaterialWidget(parent)
    , d_ptr(std::make_unique<QtMaterialListPrivate>())
{
    setMaterialComponent(QStringLiteral("List"));
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover, true);
    setAutoFillBackground(false);

    d_ptr->layout = new QVBoxLayout(this);
    d_ptr->layout->setContentsMargins(0, 0, 0, 0);
    d_ptr->layout->setSpacing(0);

    ensureSpecResolved();
    applyResolvedSpec();
}

QtMaterialList::~QtMaterialList() = default;

int QtMaterialList::count() const noexcept
{
    return d_ptr->items.size();
}

QtMaterialListItem* QtMaterialList::itemAt(
    int index) const
{
    if (
        index < 0
        || index >= d_ptr->items.size()) {
        return nullptr;
    }

    return d_ptr->items.at(index).data();
}

void QtMaterialList::addItem(
    QtMaterialListItem* item)
{
    insertItem(count(), item);
}

void QtMaterialList::insertItem(
    int index,
    QtMaterialListItem* item)
{
    if (!item) {
        return;
    }

    const int existing =
        d_ptr->items.indexOf(
            QPointer<QtMaterialListItem>(item));
    const bool inserted =
        existing < 0;

    if (existing >= 0) {
        if (existing == index) {
            return;
        }

        d_ptr->items.removeAt(existing);
        d_ptr->layout->removeWidget(item);

        if (existing < index) {
            --index;
        }
    } else if (
        auto* previousList =
            qobject_cast<QtMaterialList*>(
                item->parentWidget())) {
        if (previousList != this) {
            previousList->removeItem(item);
        }
    }

    index =
        qBound(
            0,
            index,
            d_ptr->items.size());

    if (item->parentWidget() != this) {
        item->setParent(this);
    }

    d_ptr->items.insert(index, item);
    d_ptr->layout->insertWidget(index, item);

    connect(
        item,
        &QObject::destroyed,
        this,
        &QtMaterialList::removeDestroyedItem,
        Qt::UniqueConnection);

    synchronizeItemDividers();
    updateGeometry();
    update();

    if (inserted) {
        Q_EMIT countChanged(count());
    }
}

void QtMaterialList::removeItem(
    QtMaterialListItem* item)
{
    if (!item) {
        return;
    }

    const int index =
        d_ptr->items.indexOf(
            QPointer<QtMaterialListItem>(item));

    if (index < 0) {
        return;
    }

    d_ptr->items.removeAt(index);
    d_ptr->layout->removeWidget(item);

    disconnect(
        item,
        &QObject::destroyed,
        this,
        &QtMaterialList::removeDestroyedItem);

    item->setParent(nullptr);

    synchronizeItemDividers();
    updateGeometry();
    update();

    Q_EMIT countChanged(count());
}

void QtMaterialList::clear()
{
    if (d_ptr->items.isEmpty()) {
        return;
    }

    const auto items = d_ptr->items;
    d_ptr->items.clear();

    for (
        const QPointer<QtMaterialListItem>& itemPointer
        : items) {
        QtMaterialListItem* item =
            itemPointer.data();

        if (!item) {
            continue;
        }

        d_ptr->layout->removeWidget(item);
        disconnect(
            item,
            &QObject::destroyed,
            this,
            &QtMaterialList::removeDestroyedItem);
        item->setParent(nullptr);
    }

    updateGeometry();
    update();

    Q_EMIT countChanged(0);
}

bool QtMaterialList::dividersVisible() const noexcept
{
    ensureSpecResolved();

    return d_ptr->dividersExplicit
        ? d_ptr->dividersVisible
        : d_ptr->spec.showDividers;
}

void QtMaterialList::setDividersVisible(
    bool visible)
{
    const bool oldValue =
        dividersVisible();

    d_ptr->dividersExplicit = true;
    d_ptr->dividersVisible = visible;

    if (oldValue == visible) {
        return;
    }

    synchronizeItemDividers();
    update();

    Q_EMIT dividersVisibleChanged(visible);
}

const ListSpec& QtMaterialList::resolvedSpec() const
{
    ensureSpecResolved();
    return d_ptr->spec;
}

QSize QtMaterialList::sizeHint() const
{
    ensureSpecResolved();

    const QSize contentSize =
        d_ptr->layout
            ? d_ptr->layout->sizeHint()
            : QSize();

    return contentSize.expandedTo(
        d_ptr->spec.minimumViewportSize);
}

QSize QtMaterialList::minimumSizeHint() const
{
    ensureSpecResolved();
    return d_ptr->spec.minimumViewportSize;
}

void QtMaterialList::paintEvent(
    QPaintEvent*)
{
    ensureSpecResolved();

    const QRectF visualRect =
        QRectF(rect()).adjusted(
            0.5,
            0.5,
            -0.5,
            -0.5);

    if (!visualRect.isValid()) {
        return;
    }

    const qreal radius =
        qBound<qreal>(
            0.0,
            d_ptr->spec.cornerRadius,
            qMin(
                visualRect.width(),
                visualRect.height())
                / 2.0);

    QPainterPath path;
    path.addRoundedRect(
        visualRect,
        radius,
        radius);

    QPainter painter(this);
    painter.setRenderHint(
        QPainter::Antialiasing,
        true);
    painter.fillPath(
        path,
        d_ptr->spec.containerColor);

    if (
        hasFocus()
        && d_ptr->spec.focusRingWidth > 0) {
        QPen pen(
            d_ptr->spec.focusRingColor,
            d_ptr->spec.focusRingWidth);
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(path);
    }
}

void QtMaterialList::themeChangedEvent(
    const Theme&)
{
    d_ptr->specDirty = true;
    ensureSpecResolved();
    applyResolvedSpec();
}

void QtMaterialList::ensureSpecResolved() const
{
    if (!d_ptr->specDirty) {
        return;
    }

    const DataSpecResolver resolver;
    d_ptr->spec =
        resolver.listSpec(
            theme(),
            Density::Default);
    d_ptr->specDirty = false;
}

void QtMaterialList::applyResolvedSpec()
{
    ensureSpecResolved();

    if (!d_ptr->layout) {
        return;
    }

    const QMargins margins =
        d_ptr->spec.contentMargins;

    d_ptr->layout->setContentsMargins(
        margins.left(),
        margins.top(),
        margins.right(),
        margins.bottom());
    d_ptr->layout->setSpacing(
        d_ptr->spec.itemSpacing);

    synchronizeItemDividers();
    updateGeometry();
    update();
}

void QtMaterialList::synchronizeItemDividers()
{
    const bool visible =
        dividersVisible();

    const int itemCount =
        d_ptr->items.size();

    for (int index = 0; index < itemCount; ++index) {
        QtMaterialListItem* item =
            d_ptr->items.at(index).data();

        if (!item) {
            continue;
        }

        item->setDividerVisible(
            visible
            && index + 1 < itemCount);
    }
}

void QtMaterialList::removeDestroyedItem(
    QObject* object)
{
    for (
        int index = d_ptr->items.size() - 1;
        index >= 0;
        --index) {
        if (
            d_ptr->items.at(index).isNull()
            || d_ptr->items.at(index).data() == object) {
            d_ptr->items.removeAt(index);
        }
    }

    synchronizeItemDividers();
    updateGeometry();
    update();

    Q_EMIT countChanged(count());
}

} // namespace QtMaterial
