#include "qtmaterial/widgets/inputs/qtmaterialsearchview.h"

#include <QAbstractItemModel>
#include <QListView>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>

#include "qtmaterial/widgets/inputs/qtmaterialsearchbar.h"

namespace QtMaterial {

class QtMaterialSearchViewPrivate final
{
public:
    QtMaterialSearchBar* searchBar = nullptr;
    QListView* view = nullptr;
    QSortFilterProxyModel* proxy = nullptr;
    QAbstractItemModel* sourceModel = nullptr;
};

QtMaterialSearchView::QtMaterialSearchView(QWidget* parent)
    : QWidget(parent)
    , d_ptr(std::make_unique<QtMaterialSearchViewPrivate>())
{
    d_ptr->searchBar = new QtMaterialSearchBar(this);
    d_ptr->view = new QListView(this);
    d_ptr->proxy = new QSortFilterProxyModel(this);
    setObjectName(QStringLiteral("qtmaterial_search_view"));
    setAccessibleName(tr("Search results"));

    d_ptr->proxy->setDynamicSortFilter(true);
    d_ptr->proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    d_ptr->proxy->setFilterKeyColumn(0);
    d_ptr->view->setModel(d_ptr->proxy);
    d_ptr->view->setSelectionMode(QAbstractItemView::SingleSelection);
    d_ptr->view->setUniformItemSizes(true);
    d_ptr->view->setAccessibleName(tr("Search results list"));

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);
    layout->addWidget(d_ptr->searchBar);
    layout->addWidget(d_ptr->view, 1);

    connect(d_ptr->searchBar, &QtMaterialSearchBar::textChanged, this, [this](const QString& query) {
        d_ptr->proxy->setFilterFixedString(query);
        if (d_ptr->proxy->rowCount() > 0) {
            d_ptr->view->setCurrentIndex(d_ptr->proxy->index(0, 0));
        }
        emit queryChanged(query);
    });
    connect(d_ptr->view, &QListView::activated, this, [this](const QModelIndex& proxyIndex) {
        emit activated(d_ptr->proxy->mapToSource(proxyIndex));
    });
}

QtMaterialSearchView::~QtMaterialSearchView() = default;

void QtMaterialSearchView::setSourceModel(QAbstractItemModel* model)
{
    if (d_ptr->sourceModel == model) {
        return;
    }
    d_ptr->sourceModel = model;
    d_ptr->proxy->setSourceModel(model);
}

QAbstractItemModel* QtMaterialSearchView::sourceModel() const noexcept { return d_ptr->sourceModel; }
QtMaterialSearchBar* QtMaterialSearchView::searchBar() const noexcept { return d_ptr->searchBar; }
QListView* QtMaterialSearchView::resultView() const noexcept { return d_ptr->view; }
int QtMaterialSearchView::filterKeyColumn() const noexcept { return d_ptr->proxy->filterKeyColumn(); }
void QtMaterialSearchView::setFilterKeyColumn(int column) { d_ptr->proxy->setFilterKeyColumn(column); }
Qt::CaseSensitivity QtMaterialSearchView::filterCaseSensitivity() const noexcept { return d_ptr->proxy->filterCaseSensitivity(); }
void QtMaterialSearchView::setFilterCaseSensitivity(Qt::CaseSensitivity sensitivity) { d_ptr->proxy->setFilterCaseSensitivity(sensitivity); }

} // namespace QtMaterial
