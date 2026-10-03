#include "qtmaterial/widgets/navigation/qtmaterialcommandpalette.h"

#include <QAbstractItemModel>
#include <QEvent>
#include <QKeyEvent>
#include <QLineEdit>
#include <QListView>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>

namespace QtMaterial {

class QtMaterialCommandPalettePrivate final
{
public:
    QLineEdit* searchEdit = nullptr;
    QListView* resultView = nullptr;
    QSortFilterProxyModel* proxyModel = nullptr;
};

namespace {

void syncResultState(
    QtMaterialCommandPalette* palette,
    QtMaterialCommandPalettePrivate* d)
{
    const int count = d->proxyModel->rowCount();

    if (count > 0) {
        const QModelIndex current = d->resultView->currentIndex();
        if (!current.isValid() || current.row() >= count) {
            d->resultView->setCurrentIndex(
                d->proxyModel->index(0, 0));
        }
    } else {
        d->resultView->setCurrentIndex(QModelIndex());
    }

    palette->setAccessibleDescription(
        QtMaterialCommandPalette::tr(
            "%n matching command(s)",
            nullptr,
            count));
    d->resultView->setAccessibleDescription(
        palette->accessibleDescription());
}

void moveCurrentResult(
    QtMaterialCommandPalettePrivate* d,
    int delta)
{
    const int count = d->proxyModel->rowCount();
    if (count <= 0 || delta == 0) {
        return;
    }

    int row = d->resultView->currentIndex().row();
    if (row < 0) {
        row = delta > 0 ? -1 : count;
    }
    row = (row + delta + count) % count;
    d->resultView->setCurrentIndex(
        d->proxyModel->index(row, 0));
}

} // namespace

QtMaterialCommandPalette::QtMaterialCommandPalette(QWidget* parent)
    : QDialog(parent)
    , d_ptr(std::make_unique<QtMaterialCommandPalettePrivate>())
{
    d_ptr->searchEdit = new QLineEdit(this);
    d_ptr->resultView = new QListView(this);
    d_ptr->proxyModel = new QSortFilterProxyModel(this);

    setObjectName(QStringLiteral("QtMaterialCommandPalette"));
    setAccessibleName(tr("Command palette"));
    setWindowTitle(tr("Commands"));
    setModal(true);
    resize(560, 400);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(8);

    d_ptr->searchEdit->setPlaceholderText(tr("Search commands"));
    d_ptr->searchEdit->setAccessibleName(tr("Search commands"));
    d_ptr->searchEdit->setAccessibleDescription(
        tr("Use Up and Down to navigate results, then Enter to activate."));
    d_ptr->searchEdit->installEventFilter(this);
    layout->addWidget(d_ptr->searchEdit);

    d_ptr->proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    d_ptr->proxyModel->setFilterKeyColumn(-1);
    d_ptr->resultView->setModel(d_ptr->proxyModel);
    d_ptr->resultView->setSelectionMode(QAbstractItemView::SingleSelection);
    d_ptr->resultView->setAccessibleName(tr("Command results"));
    layout->addWidget(d_ptr->resultView, 1);

    connect(d_ptr->searchEdit, &QLineEdit::textChanged, this, [this](const QString& text) {
        d_ptr->proxyModel->setFilterFixedString(text);
        syncResultState(this, d_ptr.get());
        Q_EMIT queryChanged(text);
    });
    connect(
        d_ptr->resultView,
        &QListView::activated,
        this,
        &QtMaterialCommandPalette::activateProxyIndex);
    connect(d_ptr->searchEdit, &QLineEdit::returnPressed, this, [this]() {
        QModelIndex proxyIndex = d_ptr->resultView->currentIndex();
        if (!proxyIndex.isValid() && d_ptr->proxyModel->rowCount() > 0) {
            proxyIndex = d_ptr->proxyModel->index(0, 0);
        }
        activateProxyIndex(proxyIndex);
    });

    syncResultState(this, d_ptr.get());
}

QtMaterialCommandPalette::~QtMaterialCommandPalette() = default;

void QtMaterialCommandPalette::setSourceModel(QAbstractItemModel* model)
{
    d_ptr->proxyModel->setSourceModel(model);
    syncResultState(this, d_ptr.get());
}

QAbstractItemModel* QtMaterialCommandPalette::sourceModel() const
{
    return d_ptr->proxyModel->sourceModel();
}

QString QtMaterialCommandPalette::query() const
{
    return d_ptr->searchEdit->text();
}

void QtMaterialCommandPalette::setQuery(const QString& query)
{
    d_ptr->searchEdit->setText(query);
}

bool QtMaterialCommandPalette::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == d_ptr->searchEdit
        && event
        && event->type() == QEvent::KeyPress) {
        auto* keyEvent = static_cast<QKeyEvent*>(event);
        switch (keyEvent->key()) {
        case Qt::Key_Down:
            moveCurrentResult(d_ptr.get(), 1);
            keyEvent->accept();
            return true;
        case Qt::Key_Up:
            moveCurrentResult(d_ptr.get(), -1);
            keyEvent->accept();
            return true;
        case Qt::Key_Home:
            if (d_ptr->proxyModel->rowCount() > 0) {
                d_ptr->resultView->setCurrentIndex(
                    d_ptr->proxyModel->index(0, 0));
            }
            keyEvent->accept();
            return true;
        case Qt::Key_End:
            if (d_ptr->proxyModel->rowCount() > 0) {
                d_ptr->resultView->setCurrentIndex(
                    d_ptr->proxyModel->index(
                        d_ptr->proxyModel->rowCount() - 1,
                        0));
            }
            keyEvent->accept();
            return true;
        case Qt::Key_Escape:
            reject();
            keyEvent->accept();
            return true;
        default:
            break;
        }
    }

    return QDialog::eventFilter(watched, event);
}

void QtMaterialCommandPalette::activateProxyIndex(const QModelIndex& proxyIndex)
{
    if (!proxyIndex.isValid()) {
        return;
    }

    const QModelIndex sourceIndex = d_ptr->proxyModel->mapToSource(proxyIndex);
    if (!sourceIndex.isValid()) {
        return;
    }

    Q_EMIT commandActivated(sourceIndex);
    accept();
}

} // namespace QtMaterial
