#include "qtmaterial/widgets/data/qtmaterialgridlist.h"

#include <QKeyEvent>
#include <QResizeEvent>
#include <QScrollBar>
#include <memory>

namespace QtMaterial {

struct QtMaterialGridListPrivate
{
    QtMaterialGridList::SelectionMode m_selectionMode = QtMaterialGridList::SelectionMode::SingleSelection;
    int m_columns = 3;
    QSize m_cellExtent = QSize(160, 112);
    QString m_accessibilitySummary;
};

namespace {

QString displayText(const QString& title, const QString& supportingText)
{
    if (supportingText.trimmed().isEmpty()) {
        return title;
    }
    return title + QLatin1Char('\n') + supportingText;
}

QAbstractItemView::SelectionMode toQtSelectionMode(QtMaterialGridList::SelectionMode mode)
{
    switch (mode) {
    case QtMaterialGridList::SelectionMode::NoSelection:
        return QAbstractItemView::NoSelection;
    case QtMaterialGridList::SelectionMode::MultiSelection:
        return QAbstractItemView::MultiSelection;
    case QtMaterialGridList::SelectionMode::SingleSelection:
    default:
        return QAbstractItemView::SingleSelection;
    }
}

constexpr int kTitleRole = Qt::UserRole + 120;
constexpr int kSupportingTextRole = kTitleRole + 1;

QListWidgetItem* gridItemAt(const QtMaterialGridList* self, int index)
{
    if (!self || index < 0 || index >= self->count()) {
        return nullptr;
    }
    return self->item(index);
}

int nextEnabledIndex(const QtMaterialGridList* self, int from, int delta)
{
    if (!self || self->count() == 0 || delta == 0) {
        return -1;
    }

    int candidate = from;
    if (candidate < 0) {
        candidate = delta > 0 ? -1 : self->count();
    }

    for (;;) {
        candidate += delta;
        if (candidate < 0 || candidate >= self->count()) {
            return -1;
        }
        if (self->isItemEnabled(candidate)) {
            return candidate;
        }
    }
}

int firstEnabledIndex(const QtMaterialGridList* self)
{
    if (!self) {
        return -1;
    }
    for (int i = 0; i < self->count(); ++i) {
        if (self->isItemEnabled(i)) {
            return i;
        }
    }
    return -1;
}

int lastEnabledIndex(const QtMaterialGridList* self)
{
    if (!self) {
        return -1;
    }
    for (int i = self->count() - 1; i >= 0; --i) {
        if (self->isItemEnabled(i)) {
            return i;
        }
    }
    return -1;
}

void updateItemText(QListWidgetItem* item)
{
    if (!item) {
        return;
    }

    const QString title = item->data(kTitleRole).toString();
    const QString supportingText = item->data(kSupportingTextRole).toString();
    item->setText(displayText(title, supportingText));
    item->setToolTip(displayText(title, supportingText));
}

void updateItemAccessibility(const QtMaterialGridList* self, QListWidgetItem* item)
{
    if (!self || !item) {
        return;
    }

    const int index = self->row(item);
    item->setStatusTip(self->itemAccessibleText(index));
    item->setWhatsThis(self->itemAccessibleText(index));
}

void updateGridGeometry(QtMaterialGridList* self, QtMaterialGridListPrivate* d)
{
    if (!self || !d) {
        return;
    }

    QSize gridSize = d->m_cellExtent;
    if (d->m_columns > 0 && self->viewport()) {
        const int available = qMax(0, self->viewport()->width() - self->verticalScrollBar()->sizeHint().width());
        if (available > 0) {
            const int horizontalSpacing = self->spacing() * qMax(0, d->m_columns - 1);
            const int width = qMax(d->m_cellExtent.width(), (available - horizontalSpacing) / d->m_columns);
            gridSize.setWidth(width);
        }
    }
    self->setGridSize(gridSize);
}

void syncAccessibilitySummary(QtMaterialGridList* self, QtMaterialGridListPrivate* d)
{
    if (!self || !d) {
        return;
    }

    const int selectedCount = self->selectedItems().size();
    QStringList parts;
    parts << self->tr("%n item(s)", nullptr, self->count());
    if (self->currentRow() >= 0) {
        parts << self->tr("current %1").arg(self->currentRow() + 1);
    }
    if (selectedCount > 0) {
        parts << self->tr("%n selected", nullptr, selectedCount);
    }

    const QString summary = parts.join(QStringLiteral(", "));
    if (d->m_accessibilitySummary == summary) {
        return;
    }

    d->m_accessibilitySummary = summary;
    self->setAccessibleDescription(summary);
    Q_EMIT self->accessibilitySummaryChanged(d->m_accessibilitySummary);
}

} // namespace

QtMaterialGridList::QtMaterialGridList(QWidget* parent)
    : QListWidget(parent)
{
    d_ptr = std::make_unique<QtMaterialGridListPrivate>();

    setObjectName(QStringLiteral("QtMaterialGridList"));
    setAccessibleName(QStringLiteral("Grid list"));
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    setViewMode(QListView::IconMode);
    setResizeMode(QListView::Adjust);
    setMovement(QListView::Static);
    setWrapping(true);
    setFlow(QListView::LeftToRight);
    setUniformItemSizes(true);
    setSpacing(8);
    QListWidget::setSelectionMode(toQtSelectionMode(d_ptr->m_selectionMode));

    updateGridGeometry(this, d_ptr.get());
    syncAccessibilitySummary(this, d_ptr.get());

    connect(this, &QListWidget::currentRowChanged, this, [this](int row) {
        syncAccessibilitySummary(this, d_ptr.get());
        Q_EMIT currentIndexChanged(row);
    });

    connect(this, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
        const int row = this->row(item);
        if (row >= 0 && isItemEnabled(row)) {
            Q_EMIT gridItemClicked(row);
        }
    });

    connect(this, &QListWidget::itemActivated, this, [this](QListWidgetItem* item) {
        const int row = this->row(item);
        if (row >= 0 && isItemEnabled(row)) {
            Q_EMIT gridItemActivated(row);
        }
    });

    connect(this, &QListWidget::itemSelectionChanged, this, [this]() {
        syncAccessibilitySummary(this, d_ptr.get());
        Q_EMIT gridSelectionChanged();
    });
}

QtMaterialGridList::~QtMaterialGridList() = default;

int QtMaterialGridList::addGridItem(const QString& title,
                                    const QString& supportingText,
                                    const QIcon& icon)
{
    insertGridItem(count(), title, supportingText, icon);
    return count() - 1;
}

void QtMaterialGridList::insertGridItem(int index,
                                        const QString& title,
                                        const QString& supportingText,
                                        const QIcon& icon)
{
    index = qBound(0, index, count());

    auto* item = new QListWidgetItem(icon, QString(), this);
    item->setData(kTitleRole, title);
    item->setData(kSupportingTextRole, supportingText);
    item->setSizeHint(d_ptr->m_cellExtent);
    item->setTextAlignment(Qt::AlignCenter);
    updateItemText(item);
    updateItemAccessibility(this, item);

    QListWidget::takeItem(row(item));
    QListWidget::insertItem(index, item);

    if (currentRow() < 0 && isItemEnabled(index)) {
        setCurrentRow(index);
    }

    syncAccessibilitySummary(this, d_ptr.get());
}

QString QtMaterialGridList::itemTitle(int index) const
{
    if (auto* item = gridItemAt(this, index)) {
        return item->data(kTitleRole).toString();
    }
    return QString();
}

void QtMaterialGridList::setItemTitle(int index, const QString& title)
{
    auto* item = gridItemAt(this, index);
    if (!item || item->data(kTitleRole).toString() == title) {
        return;
    }

    item->setData(kTitleRole, title);
    updateItemText(item);
    updateItemAccessibility(this, item);
    syncAccessibilitySummary(this, d_ptr.get());
}

QString QtMaterialGridList::itemSupportingText(int index) const
{
    if (auto* item = gridItemAt(this, index)) {
        return item->data(kSupportingTextRole).toString();
    }
    return QString();
}

void QtMaterialGridList::setItemSupportingText(int index, const QString& supportingText)
{
    auto* item = gridItemAt(this, index);
    if (!item || item->data(kSupportingTextRole).toString() == supportingText) {
        return;
    }

    item->setData(kSupportingTextRole, supportingText);
    updateItemText(item);
    updateItemAccessibility(this, item);
    syncAccessibilitySummary(this, d_ptr.get());
}

QIcon QtMaterialGridList::itemIcon(int index) const
{
    if (auto* item = gridItemAt(this, index)) {
        return item->icon();
    }
    return QIcon();
}

void QtMaterialGridList::setItemIcon(int index, const QIcon& icon)
{
    if (auto* item = gridItemAt(this, index)) {
        item->setIcon(icon);
    }
}

bool QtMaterialGridList::isItemEnabled(int index) const
{
    if (auto* item = gridItemAt(this, index)) {
        return item->flags().testFlag(Qt::ItemIsEnabled);
    }
    return false;
}

void QtMaterialGridList::setItemEnabled(int index, bool enabled)
{
    auto* item = gridItemAt(this, index);
    if (!item || isItemEnabled(index) == enabled) {
        return;
    }

    Qt::ItemFlags flags = item->flags();
    if (enabled) {
        flags |= Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    } else {
        flags &= ~Qt::ItemIsEnabled;
        flags &= ~Qt::ItemIsSelectable;
        item->setSelected(false);
        if (currentRow() == index) {
            setCurrentRow(nextEnabledIndex(this, index, 1));
        }
    }

    item->setFlags(flags);
    updateItemAccessibility(this, item);
    syncAccessibilitySummary(this, d_ptr.get());
}

int QtMaterialGridList::currentIndex() const
{
    return currentRow();
}

void QtMaterialGridList::setCurrentIndex(int index)
{
    if (index < 0) {
        setCurrentRow(-1);
        return;
    }

    if (!isItemEnabled(index)) {
        return;
    }

    setCurrentRow(index);
}

QtMaterialGridList::SelectionMode QtMaterialGridList::gridSelectionMode() const
{
    return d_ptr->m_selectionMode;
}

void QtMaterialGridList::setGridSelectionMode(SelectionMode mode)
{
    if (d_ptr->m_selectionMode == mode) {
        return;
    }

    d_ptr->m_selectionMode = mode;
    QListWidget::setSelectionMode(toQtSelectionMode(mode));

    if (mode == SelectionMode::NoSelection) {
        clearSelection();
    } else if (mode == SelectionMode::SingleSelection && selectedItems().size() > 1) {
        const int keep = currentRow();
        clearSelection();
        if (keep >= 0) {
            setItemSelected(keep, true);
        }
    }

    syncAccessibilitySummary(this, d_ptr.get());
    Q_EMIT gridSelectionChanged();
}

QStringList QtMaterialGridList::selectedIndexesAsStrings() const
{
    QStringList result;
    const auto indexes = selectedIndexesList();
    result.reserve(indexes.size());
    for (const int index : indexes) {
        result.push_back(QString::number(index));
    }
    return result;
}

QList<int> QtMaterialGridList::selectedIndexesList() const
{
    QList<int> result;
    const auto items = selectedItems();
    result.reserve(items.size());
    for (QListWidgetItem* item : items) {
        result.push_back(row(item));
    }
    std::sort(result.begin(), result.end());
    return result;
}

void QtMaterialGridList::setItemSelected(int index, bool selected)
{
    if (d_ptr->m_selectionMode == SelectionMode::NoSelection || !isItemEnabled(index)) {
        return;
    }

    if (auto* item = gridItemAt(this, index)) {
        item->setSelected(selected);
        syncAccessibilitySummary(this, d_ptr.get());
    }
}

void QtMaterialGridList::clearGridSelection()
{
    clearSelection();
    syncAccessibilitySummary(this, d_ptr.get());
}

int QtMaterialGridList::columns() const
{
    return d_ptr->m_columns;
}

void QtMaterialGridList::setColumns(int columns)
{
    columns = qMax(1, columns);
    if (d_ptr->m_columns == columns) {
        return;
    }

    d_ptr->m_columns = columns;
    updateGridGeometry(this, d_ptr.get());
    syncAccessibilitySummary(this, d_ptr.get());
    Q_EMIT columnsChanged(d_ptr->m_columns);
}

QSize QtMaterialGridList::cellExtent() const
{
    return d_ptr->m_cellExtent;
}

void QtMaterialGridList::setCellExtent(const QSize& size)
{
    const QSize normalized(qMax(48, size.width()), qMax(48, size.height()));
    if (d_ptr->m_cellExtent == normalized) {
        return;
    }

    d_ptr->m_cellExtent = normalized;
    for (int i = 0; i < count(); ++i) {
        if (auto* item = gridItemAt(this, i)) {
            item->setSizeHint(d_ptr->m_cellExtent);
        }
    }

    updateGridGeometry(this, d_ptr.get());
    Q_EMIT cellExtentChanged(d_ptr->m_cellExtent);
}

QString QtMaterialGridList::itemAccessibleText(int index) const
{
    const auto* item = gridItemAt(this, index);
    if (!item) {
        return QString();
    }

    QStringList parts;
    parts << itemTitle(index);

    const QString supporting = itemSupportingText(index).trimmed();
    if (!supporting.isEmpty()) {
        parts << supporting;
    }

    parts << tr("Item %1 of %2").arg(index + 1).arg(count());

    if (!isItemEnabled(index)) {
        parts << tr("Disabled");
    }

    if (item->isSelected()) {
        parts << tr("Selected");
    }

    return parts.join(QStringLiteral(", "));
}

QString QtMaterialGridList::currentItemAccessibleText() const
{
    return itemAccessibleText(currentRow());
}

QString QtMaterialGridList::accessibilitySummary() const
{
    return d_ptr->m_accessibilitySummary;
}

void QtMaterialGridList::keyPressEvent(QKeyEvent* event)
{
    if (!event) {
        return;
    }

    const int current = currentRow();
    const bool rtl = layoutDirection() == Qt::RightToLeft;

    int target = -1;
    switch (event->key()) {
    case Qt::Key_Left:
        target = nextEnabledIndex(this, current, rtl ? 1 : -1);
        break;
    case Qt::Key_Right:
        target = nextEnabledIndex(this, current, rtl ? -1 : 1);
        break;
    case Qt::Key_Up:
        target = nextEnabledIndex(this, current, -d_ptr->m_columns);
        break;
    case Qt::Key_Down:
        target = nextEnabledIndex(this, current, d_ptr->m_columns);
        break;
    case Qt::Key_Home:
        target = firstEnabledIndex(this);
        break;
    case Qt::Key_End:
        target = lastEnabledIndex(this);
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Space:
        if (current >= 0 && isItemEnabled(current)) {
            Q_EMIT gridItemActivated(current);
            event->accept();
            return;
        }
        break;
    default:
        break;
    }

    if (target >= 0) {
        setCurrentRow(target);
        event->accept();
        return;
    }

    QListWidget::keyPressEvent(event);
}

void QtMaterialGridList::resizeEvent(QResizeEvent* event)
{
    QListWidget::resizeEvent(event);
    updateGridGeometry(this, d_ptr.get());
}

} // namespace QtMaterial
