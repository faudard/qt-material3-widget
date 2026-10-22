#include "qtmaterial/widgets/data/qtmaterialselectionstate.h"

#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QDataStream>
#include <QHash>
#include <QIODevice>
#include <QItemSelectionModel>
#include <QSet>
#include <QVector>

namespace QtMaterial {
namespace {

constexpr quint32 kMagic = 0x514d5354; // QMST
constexpr quint32 kVersion = 1;
constexpr int kMaxSelections = 8192;

struct Cell {
    QString id;
    qint32 column = -1;
};

QString rowId(const QModelIndex& index, int role)
{
    return index.sibling(index.row(), 0).data(role).toString();
}

bool validCell(const Cell& cell, int columns)
{
    return !cell.id.isEmpty() && cell.column >= 0 && cell.column < columns;
}

void writeCell(QDataStream& stream, const Cell& cell)
{
    stream << cell.id << cell.column;
}

bool readCell(QDataStream& stream, Cell* cell, int columns)
{
    stream >> cell->id >> cell->column;
    return stream.status() == QDataStream::Ok && validCell(*cell, columns);
}

} // namespace

QByteArray QtMaterialSelectionState::save(
    const QAbstractItemView* view, int stableIdRole)
{
    if (!view || !view->model() || !view->selectionModel()
        || stableIdRole < Qt::UserRole) {
        return {};
    }

    const int columns = view->model()->columnCount(view->rootIndex());
    const QModelIndexList selected = view->selectionModel()->selectedIndexes();
    if (columns <= 0 || selected.size() > kMaxSelections) {
        return {};
    }

    QVector<Cell> cells;
    cells.reserve(selected.size());
    QSet<QString> seen;
    for (const QModelIndex& index : selected) {
        const Cell cell{rowId(index, stableIdRole), qint32(index.column())};
        if (!validCell(cell, columns)) {
            return {};
        }
        const QString key = cell.id + QChar(0) + QString::number(cell.column);
        if (seen.contains(key)) {
            return {};
        }
        seen.insert(key);
        cells.append(cell);
    }

    Cell current;
    if (view->currentIndex().isValid()) {
        const QModelIndex index = view->currentIndex();
        current = {rowId(index, stableIdRole), qint32(index.column())};
        if (!validCell(current, columns)) {
            return {};
        }
    }

    QByteArray state;
    QDataStream stream(&state, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_12);
    stream << kMagic << kVersion << qint32(columns)
           << qint32(cells.size());
    for (const Cell& cell : cells) {
        writeCell(stream, cell);
    }
    stream << bool(!current.id.isEmpty());
    if (!current.id.isEmpty()) {
        writeCell(stream, current);
    }
    return state;
}

bool QtMaterialSelectionState::restore(
    QAbstractItemView* view, const QByteArray& state,
    int stableIdRole, int maximumRowsToScan)
{
    if (!view || !view->model() || !view->selectionModel()
        || stableIdRole < Qt::UserRole || maximumRowsToScan <= 0
        || state.isEmpty()) {
        return false;
    }

    QDataStream stream(state);
    stream.setVersion(QDataStream::Qt_5_12);
    quint32 magic = 0;
    quint32 version = 0;
    qint32 columns = 0;
    qint32 selectedCount = 0;
    stream >> magic >> version >> columns >> selectedCount;
    if (stream.status() != QDataStream::Ok || magic != kMagic
        || version != kVersion || columns <= 0
        || columns != view->model()->columnCount(view->rootIndex())
        || selectedCount < 0 || selectedCount > kMaxSelections) {
        return false;
    }

    QVector<Cell> cells;
    cells.reserve(selectedCount);
    QSet<QString> needed;
    QSet<QString> selectedPairs;
    for (int i = 0; i < selectedCount; ++i) {
        Cell cell;
        if (!readCell(stream, &cell, columns)) {
            return false;
        }
        const QString key = cell.id + QChar(0) + QString::number(cell.column);
        if (selectedPairs.contains(key)) {
            return false;
        }
        selectedPairs.insert(key);
        needed.insert(cell.id);
        cells.append(cell);
    }

    bool hasCurrent = false;
    stream >> hasCurrent;
    Cell current;
    if (hasCurrent) {
        if (!readCell(stream, &current, columns)) {
            return false;
        }
        needed.insert(current.id);
    }
    if (stream.status() != QDataStream::Ok || !stream.atEnd()) {
        return false;
    }

    // Only visit already-loaded nodes. No fetchMore() calls or model mutation.
    const QAbstractItemModel* model = view->model();
    QVector<QModelIndex> queue;
    queue.append(view->rootIndex());
    QHash<QString, QModelIndex> resolved;
    int scanned = 0;
    for (int parentIndex = 0; parentIndex < queue.size(); ++parentIndex) {
        const QModelIndex parent = queue.at(parentIndex);
        const int rows = model->rowCount(parent);
        for (int row = 0; row < rows; ++row) {
            if (++scanned > maximumRowsToScan) {
                return false;
            }
            const QModelIndex index = model->index(row, 0, parent);
            if (!index.isValid()) {
                return false;
            }
            const QString id = rowId(index, stableIdRole);
            if (needed.contains(id)) {
                if (resolved.contains(id)) {
                    return false; // Ambiguous stable IDs.
                }
                resolved.insert(id, index);
            }
            if (model->rowCount(index) > 0) {
                queue.append(index);
            }
        }
    }
    if (resolved.size() != needed.size()) {
        return false;
    }

    QVector<QModelIndex> target;
    target.reserve(cells.size());
    for (const Cell& cell : cells) {
        const QModelIndex index = resolved.value(cell.id).sibling(
            resolved.value(cell.id).row(), cell.column);
        if (!index.isValid()) {
            return false;
        }
        target.append(index);
    }

    QModelIndex restoredCurrent;
    if (hasCurrent) {
        restoredCurrent = resolved.value(current.id).sibling(
            resolved.value(current.id).row(), current.column);
        if (!restoredCurrent.isValid()) {
            return false;
        }
    }

    QItemSelectionModel* selection = view->selectionModel();
    selection->clearSelection();
    for (const QModelIndex& index : target) {
        selection->select(index, QItemSelectionModel::Select);
    }
    selection->setCurrentIndex(restoredCurrent, QItemSelectionModel::NoUpdate);
    return true;
}

} // namespace QtMaterial
