#include "qtmaterial/widgets/navigation/qtmaterialcommandpalette.h"

#include <QAbstractItemModel>
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
    layout->addWidget(d_ptr->searchEdit);

    d_ptr->proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    d_ptr->proxyModel->setFilterKeyColumn(-1);
    d_ptr->resultView->setModel(d_ptr->proxyModel);
    d_ptr->resultView->setSelectionMode(QAbstractItemView::SingleSelection);
    layout->addWidget(d_ptr->resultView, 1);

    connect(d_ptr->searchEdit, &QLineEdit::textChanged, this, [this](const QString& text) {
        d_ptr->proxyModel->setFilterFixedString(text);
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
}

QtMaterialCommandPalette::~QtMaterialCommandPalette() = default;

void QtMaterialCommandPalette::setSourceModel(QAbstractItemModel* model)
{
    d_ptr->proxyModel->setSourceModel(model);
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
}

} // namespace QtMaterial
