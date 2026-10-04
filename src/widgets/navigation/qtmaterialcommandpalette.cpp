#include "qtmaterial/widgets/navigation/qtmaterialcommandpalette.h"

#include <algorithm>

#include <QAbstractItemModel>
#include <QAbstractListModel>
#include <QApplication>
#include <QEvent>
#include <QHash>
#include <QHideEvent>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QPainter>
#include <QPersistentModelIndex>
#include <QPointer>
#include <QShortcut>
#include <QSet>
#include <QShowEvent>
#include <QSortFilterProxyModel>
#include <QStyledItemDelegate>
#include <QStyle>
#include <QTimer>
#include <QVBoxLayout>

namespace QtMaterial {
namespace {

constexpr int kAdditionalSearchTextRole = Qt::UserRole + 0x100;

struct PaletteRow
{
    QPersistentModelIndex source;
    QPointer<QtMaterialCommandProvider> provider;
    QtMaterialCommand command;
};

class CommandModel final : public QAbstractListModel
{
public:
    explicit CommandModel(QObject* parent) : QAbstractListModel(parent) {}

    QVector<PaletteRow> commands;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override
    {
        return parent.isValid() ? 0 : commands.size();
    }

    QVariant data(const QModelIndex& index, int role) const override
    {
        if (!index.isValid() || index.row() < 0 || index.row() >= commands.size()) {
            return {};
        }
        const PaletteRow& row = commands.at(index.row());
        if (role == Qt::AccessibleTextRole) {
            return data(index, Qt::DisplayRole).toString() + QStringLiteral(". ")
                + data(index, QtMaterialCommandPalette::SecondaryTextRole).toString()
                + QStringLiteral(". ")
                + data(index, QtMaterialCommandPalette::ShortcutRole).toString();
        }
        if (row.source.isValid()) {
            if (role == kAdditionalSearchTextRole) {
                QStringList columns;
                for (int column = 1; column < row.source.model()->columnCount(row.source.parent()); ++column) {
                    columns.push_back(row.source.sibling(row.source.row(), column).data().toString());
                }
                return columns.join(QLatin1Char(' '));
            }
            return row.source.data(role);
        }
        switch (role) {
        case Qt::DisplayRole: return row.command.text;
        case Qt::DecorationRole: return row.command.icon;
        case QtMaterialCommandPalette::IdRole: return row.command.id;
        case QtMaterialCommandPalette::ShortcutRole:
            return row.command.shortcut.toString(QKeySequence::NativeText);
        case QtMaterialCommandPalette::SecondaryTextRole: return row.command.secondaryText;
        case QtMaterialCommandPalette::SectionRole: return row.command.section;
        case QtMaterialCommandPalette::KeywordsRole: return row.command.keywords;
        default: return {};
        }
    }

    Qt::ItemFlags flags(const QModelIndex& index) const override
    {
        if (!index.isValid() || index.row() >= commands.size()) {
            return Qt::NoItemFlags;
        }
        const PaletteRow& row = commands.at(index.row());
        if (row.source.isValid()) {
            return row.source.flags() & (Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        }
        return row.command.enabled && row.provider
            ? Qt::ItemIsEnabled | Qt::ItemIsSelectable : Qt::NoItemFlags;
    }

    void replace(QVector<PaletteRow> rows)
    {
        beginResetModel();
        commands = std::move(rows);
        endResetModel();
    }
};

// Ordered subsequences, with contiguous and word-boundary bonuses. The query is
// literal text, never a regular expression. Tokens may match in any word order.
int fuzzyScore(const QString& text, const QString& query)
{
    const QString folded = text.toCaseFolded();
    int score = 0;
    const QStringList tokens = query.toCaseFolded().split(QLatin1Char(' '), Qt::SkipEmptyParts);
    for (const QString& token : tokens) {
        const int substring = folded.indexOf(token);
        if (substring >= 0) {
            score += 100 + token.size() * 8 - qMin(substring, 40);
            if (substring == 0) {
                score += 60;
            }
            continue;
        }
        int position = 0;
        int previous = -2;
        for (QChar character : token) {
            const int found = folded.indexOf(character, position);
            if (found < 0) {
                return -1;
            }
            score += 8;
            if (found == previous + 1) {
                score += 12;
            }
            if (found == 0 || !folded.at(found - 1).isLetterOrNumber()) {
                score += 10;
            }
            score -= qMin(found - position, 8);
            previous = found;
            position = found + 1;
        }
    }
    return score;
}

class CommandProxy final : public QSortFilterProxyModel
{
public:
    explicit CommandProxy(QObject* parent) : QSortFilterProxyModel(parent)
    {
        setDynamicSortFilter(true);
        sort(0);
    }

    QString query;
    QStringList favorites;
    QStringList recents;
    bool fuzzy = true;

    void refresh()
    {
        scores.clear();
        invalidate();
        sort(0);
    }

    QVariant data(const QModelIndex& index, int role) const override
    {
        const QString id = QSortFilterProxyModel::data(index, QtMaterialCommandPalette::IdRole).toString();
        if (role == QtMaterialCommandPalette::FavoriteRole) {
            return !id.isEmpty() && favorites.contains(id);
        }
        if (role == QtMaterialCommandPalette::RecentRole) {
            return !id.isEmpty() && recents.contains(id);
        }
        if (role == QtMaterialCommandPalette::SectionHeadingRole) {
            const QString label = section(mapToSource(index));
            return index.row() == 0 || section(mapToSource(this->index(index.row() - 1, 0))) != label
                ? label : QString();
        }
        return QSortFilterProxyModel::data(index, role);
    }

protected:
    bool filterAcceptsRow(int row, const QModelIndex& parent) const override
    {
        return matchScore(sourceModel()->index(row, 0, parent)) >= 0;
    }

    bool lessThan(const QModelIndex& left, const QModelIndex& right) const override
    {
        const int leftGroup = group(left);
        const int rightGroup = group(right);
        if (leftGroup != rightGroup) {
            return leftGroup < rightGroup;
        }
        const QString leftSection = section(left);
        const QString rightSection = section(right);
        if (leftSection != rightSection) {
            const int folded = QString::compare(leftSection, rightSection, Qt::CaseInsensitive);
            return folded != 0 ? folded < 0 : leftSection < rightSection;
        }
        if (!query.isEmpty()) {
            const int leftScore = matchScore(left);
            const int rightScore = matchScore(right);
            if (leftScore != rightScore) {
                return leftScore > rightScore;
            }
        } else if (leftGroup == 1) {
            return recents.indexOf(left.data(QtMaterialCommandPalette::IdRole).toString())
                < recents.indexOf(right.data(QtMaterialCommandPalette::IdRole).toString());
        }
        // Preserve an application's external-model order. Provider snapshots
        // may arrive or be enumerated in any order: stable IDs break score ties.
        const auto* commands = static_cast<const CommandModel*>(sourceModel());
        if (!commands->commands.at(left.row()).source.isValid()
            && !commands->commands.at(right.row()).source.isValid()) {
            const QString leftId = left.data(QtMaterialCommandPalette::IdRole).toString();
            const QString rightId = right.data(QtMaterialCommandPalette::IdRole).toString();
            if (leftId != rightId) {
                return leftId < rightId;
            }
        }
        return left.row() < right.row();
    }

private:
    mutable QHash<int, int> scores;

    int group(const QModelIndex& index) const
    {
        const QString id = index.data(QtMaterialCommandPalette::IdRole).toString();
        if (!id.isEmpty() && favorites.contains(id)) { return 0; }
        if (!id.isEmpty() && recents.contains(id)) { return 1; }
        return 2;
    }

    QString section(const QModelIndex& index) const
    {
        if (group(index) == 0) { return QtMaterialCommandPalette::tr("Favorites"); }
        if (group(index) == 1) { return QtMaterialCommandPalette::tr("Recent commands"); }
        return index.data(QtMaterialCommandPalette::SectionRole).toString();
    }

    int matchScore(const QModelIndex& index) const
    {
        if (query.isEmpty()) { return 0; }
        const auto cached = scores.constFind(index.row());
        if (cached != scores.cend()) { return cached.value(); }
        const QString title = index.data(Qt::DisplayRole).toString();
        const QString metadata = index.data(QtMaterialCommandPalette::SecondaryTextRole).toString()
            + QLatin1Char(' ') + index.data(QtMaterialCommandPalette::KeywordsRole).toStringList().join(QLatin1Char(' '))
            + QLatin1Char(' ') + index.data(kAdditionalSearchTextRole).toString();
        int score = -1;
        if (fuzzy) {
            const int titleScore = fuzzyScore(title, query);
            const int combinedScore = fuzzyScore(title + QLatin1Char(' ') + metadata, query);
            score = titleScore >= 0 ? titleScore + 200 : combinedScore;
        } else if ((title + QLatin1Char(' ') + metadata).contains(query, Qt::CaseInsensitive)) {
            score = 0;
        }
        scores.insert(index.row(), score);
        return score;
    }
};

class CommandPaletteDelegate final : public QStyledItemDelegate
{
public:
    explicit CommandPaletteDelegate(QObject* parent) : QStyledItemDelegate(parent) {}

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        QSize size = QStyledItemDelegate::sizeHint(option, index);
        const bool secondary = !index.data(QtMaterialCommandPalette::SecondaryTextRole).toString().isEmpty();
        const bool heading = !index.data(QtMaterialCommandPalette::SectionHeadingRole).toString().isEmpty();
        size.setHeight(qMax(40, option.fontMetrics.height() * (secondary ? 2 : 1) + 16)
            + (heading ? option.fontMetrics.height() + 8 : 0));
        size.rwidth() += option.fontMetrics.horizontalAdvance(index.data(QtMaterialCommandPalette::ShortcutRole).toString()) + 32;
        return size;
    }

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        QStyleOptionViewItem item(option);
        initStyleOption(&item, index);
        painter->save();
        const QString heading = index.data(QtMaterialCommandPalette::SectionHeadingRole).toString();
        if (!heading.isEmpty()) {
            const int height = item.fontMetrics.height() + 8;
            QRect rect = item.rect;
            rect.setHeight(height);
            QFont headingFont = item.font;
            headingFont.setBold(true);
            painter->setFont(headingFont);
            painter->setPen(item.palette.color(QPalette::Text));
            painter->drawText(rect.adjusted(8, 0, -8, 0), Qt::AlignVCenter | Qt::AlignLeading, heading);
            item.rect.adjust(0, height, 0, 0);
        }
        const QString title = item.text;
        const QIcon icon = item.icon;
        item.text.clear();
        item.icon = QIcon();
        const QStyle* style = item.widget ? item.widget->style() : QApplication::style();
        style->drawControl(QStyle::CE_ItemViewItem, &item, painter, item.widget);
        const bool selected = item.state.testFlag(QStyle::State_Selected);
        const auto colorGroup = item.state.testFlag(QStyle::State_Enabled) ? QPalette::Active : QPalette::Disabled;
        painter->setPen(item.palette.color(colorGroup, selected ? QPalette::HighlightedText : QPalette::Text));
        painter->setFont(item.font);
        QRect content = item.rect.adjusted(10, 4, -10, -4);
        const QString shortcut = index.data(QtMaterialCommandPalette::ShortcutRole).toString();
        const bool favorite = index.data(QtMaterialCommandPalette::FavoriteRole).toBool();
        const QString suffix = (favorite ? QStringLiteral("★  ") : QString()) + shortcut;
        const int suffixWidth = suffix.isEmpty() ? 0 : qMin(content.width() / 3, item.fontMetrics.horizontalAdvance(suffix) + 16);
        QRect suffixRect(content.right() - suffixWidth + 1, content.top(), suffixWidth, content.height());
        suffixRect = QStyle::visualRect(item.direction, content, suffixRect);
        painter->drawText(suffixRect, Qt::AlignVCenter | Qt::AlignTrailing,
            item.fontMetrics.elidedText(suffix, Qt::ElideRight, suffixWidth));
        QRect textRect = content;
        textRect.setRight(textRect.right() - suffixWidth);
        if (!icon.isNull()) {
            const int extent = qMin(24, content.height());
            QRect iconRect(content.left(), content.center().y() - extent / 2, extent, extent);
            icon.paint(painter, QStyle::visualRect(item.direction, content, iconRect), Qt::AlignCenter,
                colorGroup == QPalette::Disabled ? QIcon::Disabled : QIcon::Normal);
            textRect.setLeft(textRect.left() + extent + 10);
        }
        textRect = QStyle::visualRect(item.direction, content, textRect);
        const QString secondary = index.data(QtMaterialCommandPalette::SecondaryTextRole).toString();
        if (secondary.isEmpty()) {
            painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeading,
                item.fontMetrics.elidedText(title, Qt::ElideRight, qMax(0, textRect.width())));
        } else {
            QRect titleRect = textRect;
            titleRect.setHeight(textRect.height() / 2);
            painter->drawText(titleRect, Qt::AlignVCenter | Qt::AlignLeading,
                item.fontMetrics.elidedText(title, Qt::ElideRight, qMax(0, titleRect.width())));
            textRect.setTop(titleRect.bottom() + 1);
            painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeading,
                item.fontMetrics.elidedText(secondary, Qt::ElideRight, qMax(0, textRect.width())));
        }
        painter->restore();
    }
};

QStringList normalizedIds(const QStringList& ids, int limit = -1)
{
    QStringList result;
    for (const QString& id : ids) {
        if (!id.isEmpty() && !result.contains(id)) {
            result.push_back(id);
            if (limit > 0 && result.size() >= limit) { break; }
        }
    }
    return result;
}

bool selectable(const QModelIndex& index)
{
    return index.isValid() && index.flags().testFlag(Qt::ItemIsEnabled)
        && index.flags().testFlag(Qt::ItemIsSelectable);
}

void moveCurrentResult(QListView* view, int delta)
{
    const int count = view->model()->rowCount();
    if (count == 0) { return; }
    const int current = view->currentIndex().row();
    int row = current >= 0 ? current : (delta > 0 ? -1 : 0);
    row = (row + delta % count + count) % count;
    for (int attempt = 0; attempt < count; ++attempt) {
        const QModelIndex index = view->model()->index(row, 0);
        if (selectable(index)) {
            view->setCurrentIndex(index);
            view->scrollTo(index);
            return;
        }
        row = (row + (delta > 0 ? 1 : -1) + count) % count;
    }
    view->setCurrentIndex({});
}

} // namespace

class QtMaterialCommandPalettePrivate final
{
public:
    struct ProviderState {
        QPointer<QtMaterialCommandProvider> provider;
        QList<QtMaterialCommand> commands;
        QList<QMetaObject::Connection> connections;
        quint64 requestId = 0;
        bool pending = false;
    };

    QLineEdit* searchEdit = nullptr;
    QListView* resultView = nullptr;
    QLabel* emptyLabel = nullptr;
    QLabel* loadingLabel = nullptr;
    CommandModel* commandModel = nullptr;
    CommandProxy* proxyModel = nullptr;
    QPointer<QAbstractItemModel> sourceModel;
    QPointer<QWidget> previousFocus;
    QList<QMetaObject::Connection> sourceConnections;
    QList<ProviderState> providers;
    QList<QKeySequence> activationShortcuts;
    QList<QPointer<QShortcut>> shortcuts;
    QTimer* providerTimer = nullptr;
    quint64 nextRequestId = 0;
    bool loading = false;
};

QtMaterialCommandProvider::QtMaterialCommandProvider(QObject* parent) : QObject(parent) {}
QtMaterialCommandProvider::~QtMaterialCommandProvider() = default;
void QtMaterialCommandProvider::cancelRequest(quint64) {}

QtMaterialCommandPalette::QtMaterialCommandPalette(QWidget* parent)
    : QDialog(parent)
    , d_ptr(std::make_unique<QtMaterialCommandPalettePrivate>())
{
    qRegisterMetaType<QList<QtMaterial::QtMaterialCommand>>();
    d_ptr->searchEdit = new QLineEdit(this);
    d_ptr->resultView = new QListView(this);
    d_ptr->emptyLabel = new QLabel(this);
    d_ptr->loadingLabel = new QLabel(tr("Searching…"), this);
    d_ptr->commandModel = new CommandModel(this);
    d_ptr->proxyModel = new CommandProxy(this);
    d_ptr->providerTimer = new QTimer(this);
    d_ptr->providerTimer->setSingleShot(true);
    d_ptr->providerTimer->setInterval(60);

    setObjectName(QStringLiteral("QtMaterialCommandPalette"));
    setAccessibleName(tr("Command palette"));
    setWindowTitle(tr("Commands"));
    setModal(true);
    resize(600, 440);
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(8);
    d_ptr->searchEdit->setPlaceholderText(tr("Search commands"));
    d_ptr->searchEdit->setAccessibleName(tr("Search commands"));
    d_ptr->searchEdit->setAccessibleDescription(tr("Use Up and Down to navigate results, Enter to activate, Ctrl+D to toggle a favorite, Escape to close."));
    d_ptr->searchEdit->installEventFilter(this);
    layout->addWidget(d_ptr->searchEdit);
    d_ptr->proxyModel->setSourceModel(d_ptr->commandModel);
    d_ptr->resultView->setModel(d_ptr->proxyModel);
    d_ptr->resultView->setSelectionMode(QAbstractItemView::SingleSelection);
    d_ptr->resultView->setAccessibleName(tr("Command results"));
    d_ptr->resultView->setItemDelegate(new CommandPaletteDelegate(d_ptr->resultView));
    d_ptr->resultView->installEventFilter(this);
    layout->addWidget(d_ptr->resultView, 1);
    d_ptr->emptyLabel->setObjectName(QStringLiteral("QtMaterialCommandPaletteEmptyState"));
    d_ptr->emptyLabel->setAlignment(Qt::AlignCenter);
    d_ptr->emptyLabel->setWordWrap(true);
    setEmptyStateText(tr("No matching commands"));
    layout->addWidget(d_ptr->emptyLabel, 1);
    d_ptr->loadingLabel->setObjectName(QStringLiteral("QtMaterialCommandPaletteLoadingState"));
    d_ptr->loadingLabel->setAccessibleName(tr("Searching commands"));
    layout->addWidget(d_ptr->loadingLabel);
    connect(d_ptr->providerTimer, &QTimer::timeout, this, &QtMaterialCommandPalette::refreshProviders);
    connect(d_ptr->searchEdit, &QLineEdit::textChanged, this, [this](const QString& text) {
        cancelProviderRequests();
        for (auto& state : d_ptr->providers) { state.commands.clear(); }
        d_ptr->proxyModel->query = text.simplified();
        d_ptr->resultView->setCurrentIndex({});
        rebuildCommands();
        if (!d_ptr->providers.isEmpty()) { d_ptr->providerTimer->start(); }
        updateLoading();
        Q_EMIT queryChanged(text);
    });
    connect(d_ptr->resultView, &QListView::activated, this, &QtMaterialCommandPalette::activateProxyIndex);
    setActivationShortcuts({QKeySequence(Qt::CTRL | Qt::Key_K), QKeySequence(Qt::CTRL | Qt::Key_P)});
    syncResults();
}

QtMaterialCommandPalette::~QtMaterialCommandPalette()
{
    cancelProviderRequests();
    for (const auto& shortcut : d_ptr->shortcuts) { delete shortcut.data(); }
}

void QtMaterialCommandPalette::setSourceModel(QAbstractItemModel* model)
{
    if (d_ptr->sourceModel == model) { return; }
    for (const auto& connection : d_ptr->sourceConnections) { disconnect(connection); }
    d_ptr->sourceConnections.clear();
    d_ptr->sourceModel = model;
    if (model) {
        const auto rebuild = [this]() { rebuildCommands(); };
        d_ptr->sourceConnections = {
            connect(model, &QAbstractItemModel::modelReset, this, rebuild),
            connect(model, &QAbstractItemModel::layoutChanged, this, rebuild),
            connect(model, &QAbstractItemModel::rowsInserted, this, rebuild),
            connect(model, &QAbstractItemModel::rowsRemoved, this, rebuild),
            connect(model, &QAbstractItemModel::rowsMoved, this, rebuild),
            connect(model, &QAbstractItemModel::columnsInserted, this, rebuild),
            connect(model, &QAbstractItemModel::columnsRemoved, this, rebuild),
            connect(model, &QAbstractItemModel::dataChanged, this, rebuild),
            connect(model, &QObject::destroyed, this, rebuild)
        };
    }
    rebuildCommands();
}

QAbstractItemModel* QtMaterialCommandPalette::sourceModel() const { return d_ptr->sourceModel; }

void QtMaterialCommandPalette::addProvider(QtMaterialCommandProvider* provider)
{
    if (!provider || providers().contains(provider)) { return; }
    QtMaterialCommandPalettePrivate::ProviderState state;
    state.provider = provider;
    state.connections = {
        connect(provider, &QtMaterialCommandProvider::commandsReady, this,
            [this, provider](quint64 requestId, const QList<QtMaterialCommand>& commands) {
                for (auto& current : d_ptr->providers) {
                    if (current.provider == provider && current.pending && current.requestId == requestId) {
                        current.pending = false;
                        current.commands = commands;
                        rebuildCommands();
                        updateLoading();
                        return;
                    }
                }
            }),
        connect(provider, &QtMaterialCommandProvider::requestFailed, this,
            [this, provider](quint64 requestId, const QString& message) {
                for (auto& current : d_ptr->providers) {
                    if (current.provider == provider && current.pending && current.requestId == requestId) {
                        current.pending = false;
                        current.commands.clear();
                        rebuildCommands();
                        updateLoading();
                        Q_EMIT providerFailed(provider, message);
                        return;
                    }
                }
            }),
        connect(provider, &QObject::destroyed, this, [this]() {
            for (int i = d_ptr->providers.size() - 1; i >= 0; --i) {
                if (!d_ptr->providers.at(i).provider) { d_ptr->providers.removeAt(i); }
            }
            rebuildCommands();
            updateLoading();
        })
    };
    d_ptr->providers.push_back(state);
    refreshProviders();
}

void QtMaterialCommandPalette::removeProvider(QtMaterialCommandProvider* provider)
{
    for (int i = d_ptr->providers.size() - 1; i >= 0; --i) {
        const auto state = d_ptr->providers.at(i);
        if (state.provider == provider) {
            d_ptr->providers.removeAt(i);
            for (const auto& connection : state.connections) { disconnect(connection); }
            if (state.pending && state.provider) {
                QMetaObject::invokeMethod(provider, [guard = state.provider, id = state.requestId]() {
                    if (guard) { guard->cancelRequest(id); }
                }, Qt::AutoConnection);
            }
        }
    }
    rebuildCommands();
    updateLoading();
}

QList<QtMaterialCommandProvider*> QtMaterialCommandPalette::providers() const
{
    QList<QtMaterialCommandProvider*> result;
    for (const auto& state : d_ptr->providers) {
        if (state.provider) { result.push_back(state.provider); }
    }
    return result;
}

void QtMaterialCommandPalette::cancelProviderRequests()
{
    d_ptr->providerTimer->stop();
    const auto states = d_ptr->providers;
    for (auto& state : d_ptr->providers) { state.pending = false; state.requestId = 0; }
    for (const auto& state : states) {
        if (state.pending && state.provider) {
            QMetaObject::invokeMethod(state.provider, [guard = state.provider, id = state.requestId]() {
                if (guard) { guard->cancelRequest(id); }
            }, Qt::AutoConnection);
        }
    }
}

void QtMaterialCommandPalette::refreshProviders()
{
    cancelProviderRequests();
    const QString text = query();
    for (auto& state : d_ptr->providers) {
        state.commands.clear();
        state.requestId = ++d_ptr->nextRequestId;
        state.pending = true;
    }
    // Take a snapshot before invoking application code; synchronous providers
    // may remove themselves or add another provider while publishing results.
    const auto requests = d_ptr->providers;
    rebuildCommands();
    updateLoading();
    for (const auto& request : requests) {
        if (!request.provider) { continue; }
        bool active = false;
        for (const auto& state : d_ptr->providers) {
            active |= state.provider == request.provider && state.requestId == request.requestId && state.pending;
        }
        if (active) {
            QMetaObject::invokeMethod(request.provider, [guard = request.provider, text, id = request.requestId]() {
                if (guard) { guard->requestCommands(text, id); }
            }, Qt::AutoConnection);
        }
    }
}

void QtMaterialCommandPalette::rebuildCommands()
{
    const QModelIndex current = d_ptr->resultView->currentIndex();
    const QString selectedId = current.data(IdRole).toString();
    QPersistentModelIndex selectedSource;
    if (current.isValid()) {
        const QModelIndex source = d_ptr->proxyModel->mapToSource(current);
        if (source.isValid()) { selectedSource = d_ptr->commandModel->commands.at(source.row()).source; }
    }
    QVector<PaletteRow> rows;
    QSet<QString> ids;
    if (d_ptr->sourceModel) {
        for (int row = 0; row < d_ptr->sourceModel->rowCount(); ++row) {
            PaletteRow command;
            command.source = d_ptr->sourceModel->index(row, 0);
            if (!command.source.isValid()) { continue; }
            const QString id = command.source.data(IdRole).toString();
            if (!id.isEmpty()) { ids.insert(id); }
            rows.push_back(command);
        }
    }
    for (const auto& state : d_ptr->providers) {
        if (!state.provider) { continue; }
        for (const auto& command : state.commands) {
            if (command.id.isEmpty() || command.text.isEmpty() || ids.contains(command.id)) { continue; }
            ids.insert(command.id);
            rows.push_back({{}, state.provider, command});
        }
    }
    // Drop score caches before modelReset triggers proxy filtering/sorting.
    d_ptr->proxyModel->refresh();
    d_ptr->commandModel->replace(std::move(rows));
    d_ptr->proxyModel->refresh();
    for (int i = 0; i < d_ptr->commandModel->commands.size(); ++i) {
        const auto& command = d_ptr->commandModel->commands.at(i);
        const QModelIndex source = d_ptr->commandModel->index(i, 0);
        if ((!selectedId.isEmpty() && source.data(IdRole).toString() == selectedId)
            || (selectedSource.isValid() && command.source == selectedSource)) {
            const QModelIndex restored = d_ptr->proxyModel->mapFromSource(source);
            if (selectable(restored)) { d_ptr->resultView->setCurrentIndex(restored); }
            break;
        }
    }
    syncResults();
}

void QtMaterialCommandPalette::syncResults()
{
    const int count = d_ptr->proxyModel->rowCount();
    if (!selectable(d_ptr->resultView->currentIndex())) {
        d_ptr->resultView->setCurrentIndex({});
        moveCurrentResult(d_ptr->resultView, 1);
    }
    d_ptr->resultView->setVisible(count > 0);
    d_ptr->emptyLabel->setVisible(count == 0 && !isLoading());
    d_ptr->loadingLabel->setVisible(isLoading());
    setAccessibleDescription(tr("%n matching command(s)", nullptr, count)
        + (isLoading() ? tr("; searching") : QString()));
    d_ptr->resultView->setAccessibleDescription(accessibleDescription());
}

void QtMaterialCommandPalette::updateLoading()
{
    bool pending = d_ptr->providerTimer->isActive();
    for (const auto& state : d_ptr->providers) { pending |= state.pending; }
    const bool changed = d_ptr->loading != pending;
    d_ptr->loading = pending;
    syncResults();
    if (changed) { Q_EMIT loadingChanged(pending); }
}

bool QtMaterialCommandPalette::fuzzyMatchingEnabled() const { return d_ptr->proxyModel->fuzzy; }
void QtMaterialCommandPalette::setFuzzyMatchingEnabled(bool enabled)
{
    if (d_ptr->proxyModel->fuzzy == enabled) { return; }
    d_ptr->proxyModel->fuzzy = enabled;
    d_ptr->proxyModel->refresh();
    syncResults();
}
bool QtMaterialCommandPalette::isLoading() const { return d_ptr->loading; }
QStringList QtMaterialCommandPalette::favoriteCommandIds() const { return d_ptr->proxyModel->favorites; }
void QtMaterialCommandPalette::setFavoriteCommandIds(const QStringList& ids)
{
    const QStringList normalized = normalizedIds(ids);
    if (favoriteCommandIds() == normalized) { return; }
    d_ptr->proxyModel->favorites = normalized;
    d_ptr->proxyModel->refresh();
    syncResults();
    Q_EMIT favoriteCommandIdsChanged(normalized);
}
void QtMaterialCommandPalette::setCommandFavorite(const QString& id, bool favorite)
{
    if (id.isEmpty()) { return; }
    QStringList ids = favoriteCommandIds();
    ids.removeAll(id);
    if (favorite) { ids.push_back(id); }
    setFavoriteCommandIds(ids);
}
QStringList QtMaterialCommandPalette::recentCommandIds() const { return d_ptr->proxyModel->recents; }
void QtMaterialCommandPalette::setRecentCommandIds(const QStringList& ids)
{
    const QStringList normalized = normalizedIds(ids, 20);
    if (recentCommandIds() == normalized) { return; }
    d_ptr->proxyModel->recents = normalized;
    d_ptr->proxyModel->refresh();
    syncResults();
    Q_EMIT recentCommandIdsChanged(normalized);
}
void QtMaterialCommandPalette::clearRecentCommands() { setRecentCommandIds({}); }
QList<QKeySequence> QtMaterialCommandPalette::activationShortcuts() const { return d_ptr->activationShortcuts; }
void QtMaterialCommandPalette::setActivationShortcuts(const QList<QKeySequence>& shortcuts)
{
    for (const auto& shortcut : d_ptr->shortcuts) { delete shortcut.data(); }
    d_ptr->shortcuts.clear();
    d_ptr->activationShortcuts.clear();
    QWidget* host = parentWidget() ? parentWidget()->window() : this;
    for (const QKeySequence& key : shortcuts) {
        if (key.isEmpty() || d_ptr->activationShortcuts.contains(key)) { continue; }
        auto* shortcut = new QShortcut(key, host);
        shortcut->setContext(Qt::WindowShortcut);
        connect(shortcut, &QShortcut::activated, this, &QtMaterialCommandPalette::openPalette);
        d_ptr->shortcuts.push_back(shortcut);
        d_ptr->activationShortcuts.push_back(key);
    }
}

void QtMaterialCommandPalette::openPalette()
{
    if (!isVisible()) {
        QWidget* focused = QApplication::focusWidget();
        d_ptr->previousFocus = focused && focused != this && !isAncestorOf(focused) ? focused : nullptr;
    }
    setQuery(QString());
    show();
    raise();
    activateWindow();
    d_ptr->searchEdit->setFocus(Qt::ShortcutFocusReason);
    d_ptr->searchEdit->selectAll();
}
QString QtMaterialCommandPalette::query() const { return d_ptr->searchEdit->text(); }
void QtMaterialCommandPalette::setQuery(const QString& query) { d_ptr->searchEdit->setText(query); }
QString QtMaterialCommandPalette::emptyStateText() const { return d_ptr->emptyLabel->text(); }
void QtMaterialCommandPalette::setEmptyStateText(const QString& text)
{
    d_ptr->emptyLabel->setText(text);
    d_ptr->emptyLabel->setAccessibleName(text);
}

bool QtMaterialCommandPalette::eventFilter(QObject* watched, QEvent* event)
{
    if ((watched == d_ptr->searchEdit || watched == d_ptr->resultView) && event->type() == QEvent::KeyPress) {
        auto* key = static_cast<QKeyEvent*>(event);
        if (key->key() == Qt::Key_D && key->modifiers().testFlag(Qt::ControlModifier)) {
            const QString id = d_ptr->resultView->currentIndex().data(IdRole).toString();
            setCommandFavorite(id, !favoriteCommandIds().contains(id));
            return true;
        }
        switch (key->key()) {
        case Qt::Key_Down: moveCurrentResult(d_ptr->resultView, 1); return true;
        case Qt::Key_Up: moveCurrentResult(d_ptr->resultView, -1); return true;
        case Qt::Key_PageDown:
        case Qt::Key_PageUp: {
            const int page = qMax(1, d_ptr->resultView->viewport()->height() / 48);
            moveCurrentResult(d_ptr->resultView, key->key() == Qt::Key_PageDown ? page : -page);
            return true;
        }
        case Qt::Key_Home:
        case Qt::Key_End:
            d_ptr->resultView->setCurrentIndex({});
            moveCurrentResult(d_ptr->resultView, key->key() == Qt::Key_Home ? 1 : -1);
            return true;
        case Qt::Key_Return:
        case Qt::Key_Enter: activateProxyIndex(d_ptr->resultView->currentIndex()); return true;
        case Qt::Key_Space:
            if (watched == d_ptr->resultView) {
                activateProxyIndex(d_ptr->resultView->currentIndex());
                return true;
            }
            break;
        case Qt::Key_Escape: reject(); return true;
        default: break;
        }
    }
    return QDialog::eventFilter(watched, event);
}

void QtMaterialCommandPalette::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);
    refreshProviders();
    d_ptr->searchEdit->setFocus(Qt::OtherFocusReason);
}
void QtMaterialCommandPalette::hideEvent(QHideEvent* event)
{
    const QPointer<QWidget> previousFocus = d_ptr->previousFocus;
    d_ptr->previousFocus.clear();
    cancelProviderRequests();
    updateLoading();
    QDialog::hideEvent(event);
    if (previousFocus && previousFocus->isVisible() && previousFocus->isEnabled()) {
        previousFocus->window()->activateWindow();
        if (previousFocus) { previousFocus->setFocus(Qt::OtherFocusReason); }
    }
}

void QtMaterialCommandPalette::activateProxyIndex(const QModelIndex& proxyIndex)
{
    if (!selectable(proxyIndex)) { return; }
    const QModelIndex index = d_ptr->proxyModel->mapToSource(proxyIndex);
    if (!index.isValid()) { return; }
    const PaletteRow row = d_ptr->commandModel->commands.at(index.row());
    const QPointer<QtMaterialCommandPalette> guard(this);
    const QString id = proxyIndex.data(IdRole).toString();
    if (!id.isEmpty()) {
        QStringList recents = recentCommandIds();
        recents.removeAll(id);
        recents.prepend(id);
        setRecentCommandIds(recents);
    }
    if (!guard) { return; }
    accept();
    if (!guard) { return; }
    if (row.source.isValid()) {
        Q_EMIT commandActivated(row.source);
    } else if (row.provider) {
        Q_EMIT providerCommandActivated(row.provider, id);
        if (!row.provider) { return; }
        QMetaObject::invokeMethod(row.provider, [provider = row.provider, id]() {
            if (provider) { provider->activateCommand(id); }
        }, Qt::AutoConnection);
    }
}

} // namespace QtMaterial
