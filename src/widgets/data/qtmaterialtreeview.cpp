#include "qtmaterial/widgets/data/qtmaterialtreeview.h"

#include <QAbstractItemView>
#include <QAbstractItemModel>
#include <QDataStream>
#include <QHeaderView>
#include <QIODevice>
#include <QItemSelectionModel>
#include <QVector>

namespace QtMaterial {
namespace {

using RowPath = QVector<int>;

bool rowPathForIndex(
    const QModelIndex& root,
    const QModelIndex& index,
    RowPath* path)
{
    if (!path) {
        return false;
    }

    path->clear();
    if (!index.isValid()) {
        return true;
    }

    QModelIndex cursor =
        index.sibling(index.row(), 0);
    while (cursor.isValid()) {
        path->prepend(cursor.row());
        const QModelIndex parent = cursor.parent();
        if (parent == root) {
            return true;
        }
        cursor = parent;
    }

    path->clear();
    return false;
}

QModelIndex indexForRowPath(
    const QAbstractItemModel* model,
    const QModelIndex& root,
    const RowPath& path,
    int finalColumn)
{
    if (!model || path.isEmpty() || finalColumn < 0) {
        return {};
    }

    QModelIndex parent = root;
    QModelIndex result;
    for (int depth = 0; depth < path.size(); ++depth) {
        const int row = path.at(depth);
        const int column =
            depth == path.size() - 1
                ? finalColumn
                : 0;
        if (
            row < 0
            || row >= model->rowCount(parent)
            || column < 0
            || column >= model->columnCount(parent)) {
            return {};
        }

        result = model->index(row, column, parent);
        if (!result.isValid()) {
            return {};
        }

        if (depth != path.size() - 1) {
            parent = model->index(row, 0, parent);
            if (!parent.isValid()) {
                return {};
            }
        }
    }

    return result;
}

void collectExpandedRowPaths(
    const QTreeView* tree,
    const QAbstractItemModel* model,
    const QModelIndex& parent,
    const RowPath& prefix,
    QVector<RowPath>* paths)
{
    if (!tree || !model || !paths) {
        return;
    }

    const int rows = model->rowCount(parent);
    for (int row = 0; row < rows; ++row) {
        const QModelIndex index =
            model->index(row, 0, parent);
        if (!index.isValid() || !tree->isExpanded(index)) {
            continue;
        }

        RowPath path = prefix;
        path.push_back(row);
        paths->push_back(path);
        collectExpandedRowPaths(
            tree,
            model,
            index,
            path,
            paths);
    }
}

bool readRowPath(
    QDataStream* stream,
    RowPath* path)
{
    if (!stream || !path) {
        return false;
    }

    qint32 size = 0;
    *stream >> size;
    if (
        stream->status() != QDataStream::Ok
        || size < 0
        || size > 1024) {
        return false;
    }

    path->clear();
    path->reserve(size);
    for (qint32 index = 0; index < size; ++index) {
        qint32 row = -1;
        *stream >> row;
        if (
            stream->status() != QDataStream::Ok
            || row < 0) {
            path->clear();
            return false;
        }
        path->push_back(row);
    }
    return true;
}

void writeRowPath(
    QDataStream* stream,
    const RowPath& path)
{
    *stream << qint32(path.size());
    for (int row : path) {
        *stream << qint32(row);
    }
}

} // namespace

class QtMaterialTreeViewPrivate final
{
public:
    bool dense = false;
    QString accessibilitySummary;
};

QtMaterialTreeView::QtMaterialTreeView(QWidget* parent)
    : QTreeView(parent)
    , d_ptr(std::make_unique<QtMaterialTreeViewPrivate>())
{
    setObjectName(QStringLiteral("QtMaterialTreeView"));
    setAccessibleName(tr("Tree"));
    setFocusPolicy(Qt::StrongFocus);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setUniformRowHeights(true);
    setAnimated(true);
    setAllColumnsShowFocus(true);
    setExpandsOnDoubleClick(true);
    setIndentation(20);
    setDragDropMode(QAbstractItemView::NoDragDrop);
    setEditTriggers(
        QAbstractItemView::DoubleClicked
        | QAbstractItemView::EditKeyPressed);
    connect(
        viewport(),
        &QWidget::customContextMenuRequested,
        this,
        [this](const QPoint& position) {
            Q_EMIT contextMenuRequested(
                indexAt(position),
                viewport()->mapToGlobal(position));
        });
    connect(this, &QTreeView::expanded, this, [this](const QModelIndex&) { syncAccessibility(); });
    connect(this, &QTreeView::collapsed, this, [this](const QModelIndex&) { syncAccessibility(); });
    syncAccessibility();
}

QtMaterialTreeView::~QtMaterialTreeView() = default;

bool QtMaterialTreeView::dense() const noexcept
{
    return d_ptr->dense;
}

void QtMaterialTreeView::setDense(bool dense)
{
    if (d_ptr->dense == dense) {
        return;
    }

    d_ptr->dense = dense;
    setIndentation(d_ptr->dense ? 16 : 20);
    setIconSize(d_ptr->dense ? QSize(18, 18) : QSize(20, 20));
    viewport()->update();
    Q_EMIT denseChanged(d_ptr->dense);
}

bool QtMaterialTreeView::multiSelectionEnabled() const noexcept
{
    return selectionMode() == QAbstractItemView::ExtendedSelection
        || selectionMode() == QAbstractItemView::MultiSelection;
}

void QtMaterialTreeView::setMultiSelectionEnabled(bool enabled)
{
    if (multiSelectionEnabled() == enabled) {
        return;
    }

    setSelectionMode(
        enabled
            ? QAbstractItemView::ExtendedSelection
            : QAbstractItemView::SingleSelection);
    Q_EMIT multiSelectionEnabledChanged(enabled);
}

bool QtMaterialTreeView::dragDropEnabled() const noexcept
{
    return dragDropMode() == QAbstractItemView::InternalMove;
}

QString QtMaterialTreeView::accessibilitySummary() const
{
    return d_ptr->accessibilitySummary;
}

QString QtMaterialTreeView::currentItemAccessibleText() const
{
    const QModelIndex index = currentIndex();
    if (!index.isValid()) {
        return QString();
    }

    QString text = index.data(Qt::AccessibleTextRole).toString();
    if (text.isEmpty()) {
        text = index.data(Qt::DisplayRole).toString();
    }

    QString state;
    if (model() && model()->hasChildren(index)) {
        state = isExpanded(index)
            ? tr("expanded")
            : tr("collapsed");
    }

    QString result = tr("Row %1, column %2").arg(index.row() + 1).arg(index.column() + 1);
    if (!text.isEmpty()) {
        result += QStringLiteral(", ") + text;
    }
    if (!state.isEmpty()) {
        result += QStringLiteral(", ") + state;
    }
    return result;
}

QByteArray QtMaterialTreeView::saveWorkspaceState() const
{
    constexpr quint32 magic = 0x514d5257; // QMRW
    constexpr quint32 version = 1;

    QByteArray state;
    QDataStream stream(&state, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_12);

    const QHeaderView* treeHeader = header();
    stream
        << magic
        << version
        << qint32(treeHeader ? treeHeader->count() : 0)
        << (treeHeader ? treeHeader->saveState() : QByteArray())
        << dense()
        << multiSelectionEnabled()
        << dragDropEnabled()
        << inlineEditingEnabled()
        << contextMenuEnabled();

    RowPath currentPath;
    int currentColumn = -1;
    if (
        currentIndex().isValid()
        && rowPathForIndex(
            rootIndex(),
            currentIndex(),
            &currentPath)) {
        currentColumn = currentIndex().column();
    } else {
        currentPath.clear();
    }
    writeRowPath(&stream, currentPath);
    stream << qint32(currentColumn);

    QVector<RowPath> expandedPaths;
    if (model()) {
        collectExpandedRowPaths(
            this,
            model(),
            rootIndex(),
            {},
            &expandedPaths);
    }

    stream << qint32(expandedPaths.size());
    for (const RowPath& path : expandedPaths) {
        writeRowPath(&stream, path);
    }

    return state;
}

bool QtMaterialTreeView::restoreWorkspaceState(
    const QByteArray& state)
{
    constexpr quint32 magic = 0x514d5257; // QMRW
    constexpr quint32 version = 1;

    QDataStream stream(state);
    stream.setVersion(QDataStream::Qt_5_12);

    quint32 storedMagic = 0;
    quint32 storedVersion = 0;
    qint32 sectionCount = 0;
    QByteArray headerState;
    bool storedDense = false;
    bool storedMultiSelection = false;
    bool storedDragDrop = false;
    bool storedInlineEditing = false;
    bool storedContextMenu = false;

    stream
        >> storedMagic
        >> storedVersion
        >> sectionCount
        >> headerState
        >> storedDense
        >> storedMultiSelection
        >> storedDragDrop
        >> storedInlineEditing
        >> storedContextMenu;

    RowPath currentPath;
    if (!readRowPath(&stream, &currentPath)) {
        return false;
    }

    qint32 currentColumn = -1;
    qint32 expandedCount = 0;
    stream >> currentColumn >> expandedCount;
    if (
        stream.status() != QDataStream::Ok
        || expandedCount < 0
        || expandedCount > 100000) {
        return false;
    }

    QVector<RowPath> expandedPaths;
    expandedPaths.reserve(expandedCount);
    for (qint32 index = 0; index < expandedCount; ++index) {
        RowPath path;
        if (!readRowPath(&stream, &path) || path.isEmpty()) {
            return false;
        }
        expandedPaths.push_back(path);
    }

    if (
        stream.status() != QDataStream::Ok
        || !stream.atEnd()
        || storedMagic != magic
        || storedVersion != version
        || sectionCount < 0
        || (currentPath.isEmpty() && currentColumn != -1)
        || (!currentPath.isEmpty() && currentColumn < 0)) {
        return false;
    }

    QHeaderView* treeHeader = header();
    if (!treeHeader || treeHeader->count() != sectionCount) {
        return false;
    }

    const QAbstractItemModel* currentModel = model();
    if (
        (!currentPath.isEmpty() || !expandedPaths.isEmpty())
        && !currentModel) {
        return false;
    }

    QModelIndex restoredCurrent;
    if (!currentPath.isEmpty()) {
        restoredCurrent =
            indexForRowPath(
                currentModel,
                rootIndex(),
                currentPath,
                currentColumn);
        if (!restoredCurrent.isValid()) {
            return false;
        }
    }

    QVector<QModelIndex> restoredExpanded;
    restoredExpanded.reserve(expandedPaths.size());
    for (const RowPath& path : expandedPaths) {
        const QModelIndex index =
            indexForRowPath(
                currentModel,
                rootIndex(),
                path,
                0);
        if (!index.isValid()) {
            return false;
        }
        restoredExpanded.push_back(index);
    }

    const QByteArray previousHeaderState =
        treeHeader->saveState();
    if (!treeHeader->restoreState(headerState)) {
        treeHeader->restoreState(previousHeaderState);
        return false;
    }

    setDense(storedDense);
    setMultiSelectionEnabled(storedMultiSelection);
    setDragDropEnabled(storedDragDrop);
    setInlineEditingEnabled(storedInlineEditing);
    setContextMenuEnabled(storedContextMenu);

    collapseAll();
    for (const QModelIndex& index : restoredExpanded) {
        setExpanded(index, true);
    }
    if (restoredCurrent.isValid()) {
        setCurrentIndex(restoredCurrent);
        scrollTo(restoredCurrent);
    } else {
        setCurrentIndex(QModelIndex());
    }

    syncAccessibility();
    return true;
}

void QtMaterialTreeView::setModel(QAbstractItemModel* model)
{
    if (this->model()) {
        disconnect(this->model(), nullptr, this, nullptr);
    }

    QTreeView::setModel(model);

    if (selectionModel()) {
        connect(
            selectionModel(),
            &QItemSelectionModel::currentChanged,
            this,
            [this](const QModelIndex&, const QModelIndex&) {
                syncAccessibility();
            });
    }

    if (model) {
        connect(model, &QAbstractItemModel::modelReset, this, &QtMaterialTreeView::syncAccessibility);
        connect(model, &QAbstractItemModel::rowsInserted, this, &QtMaterialTreeView::syncAccessibility);
        connect(model, &QAbstractItemModel::rowsRemoved, this, &QtMaterialTreeView::syncAccessibility);
        connect(model, &QAbstractItemModel::dataChanged, this, [this]() { syncAccessibility(); });
    }

    syncAccessibility();
}

void QtMaterialTreeView::currentChanged(
    const QModelIndex& current,
    const QModelIndex& previous)
{
    QTreeView::currentChanged(current, previous);
    syncAccessibility();
}

void QtMaterialTreeView::syncAccessibility()
{
    const int roots = model() ? model()->rowCount(rootIndex()) : 0;
    QString summary = tr("%1 top-level items").arg(roots);
    const QString current = currentItemAccessibleText();
    if (!current.isEmpty()) {
        summary += QStringLiteral(". ") + current;
    }

    if (d_ptr->accessibilitySummary == summary) {
        return;
    }

    d_ptr->accessibilitySummary = summary;
    setAccessibleDescription(summary);
    Q_EMIT accessibilitySummaryChanged(summary);
}

void QtMaterialTreeView::setDragDropEnabled(bool enabled)
{
    if (dragDropEnabled() == enabled) {
        return;
    }

    setDragEnabled(enabled);
    viewport()->setAcceptDrops(enabled);
    setDropIndicatorShown(enabled);
    setDefaultDropAction(Qt::MoveAction);
    setDragDropMode(
        enabled
            ? QAbstractItemView::InternalMove
            : QAbstractItemView::NoDragDrop);
    Q_EMIT dragDropEnabledChanged(enabled);
}

bool QtMaterialTreeView::inlineEditingEnabled() const noexcept
{
    return editTriggers() != QAbstractItemView::NoEditTriggers;
}

void QtMaterialTreeView::setInlineEditingEnabled(bool enabled)
{
    if (inlineEditingEnabled() == enabled) {
        return;
    }

    setEditTriggers(
        enabled
            ? QAbstractItemView::EditTriggers(
                QAbstractItemView::DoubleClicked
                | QAbstractItemView::EditKeyPressed)
            : QAbstractItemView::NoEditTriggers);
    Q_EMIT inlineEditingEnabledChanged(enabled);
}

bool QtMaterialTreeView::contextMenuEnabled() const noexcept
{
    return viewport()->contextMenuPolicy() == Qt::CustomContextMenu;
}

void QtMaterialTreeView::setContextMenuEnabled(bool enabled)
{
    if (contextMenuEnabled() == enabled) {
        return;
    }

    viewport()->setContextMenuPolicy(
        enabled
            ? Qt::CustomContextMenu
            : Qt::DefaultContextMenu);
    Q_EMIT contextMenuEnabledChanged(enabled);
}

} // namespace QtMaterial
