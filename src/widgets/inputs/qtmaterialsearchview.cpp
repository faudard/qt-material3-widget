#include "qtmaterial/widgets/inputs/qtmaterialsearchview.h"

#include <QAbstractItemModel>
#include <QListView>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>

#include "qtmaterial/widgets/inputs/qtmaterialsearchbar.h"

namespace QtMaterial {

QtMaterialSearchView::QtMaterialSearchView(QWidget* parent)
    : QWidget(parent)
    , m_searchBar(new QtMaterialSearchBar(this))
    , m_view(new QListView(this))
    , m_proxy(new QSortFilterProxyModel(this))
{
    setObjectName(QStringLiteral("qtmaterial_search_view"));
    setAccessibleName(tr("Search results"));

    m_proxy->setDynamicSortFilter(true);
    m_proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxy->setFilterKeyColumn(0);
    m_view->setModel(m_proxy);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setUniformItemSizes(true);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);
    layout->addWidget(m_searchBar);
    layout->addWidget(m_view, 1);

    connect(m_searchBar, &QtMaterialSearchBar::textChanged, this, [this](const QString& query) {
        m_proxy->setFilterFixedString(query);
        if (m_proxy->rowCount() > 0) {
            m_view->setCurrentIndex(m_proxy->index(0, 0));
        }
        emit queryChanged(query);
    });
    connect(m_view, &QListView::activated, this, [this](const QModelIndex& proxyIndex) {
        emit activated(m_proxy->mapToSource(proxyIndex));
    });
}

QtMaterialSearchView::~QtMaterialSearchView() = default;

void QtMaterialSearchView::setSourceModel(QAbstractItemModel* model)
{
    if (m_sourceModel == model) {
        return;
    }
    m_sourceModel = model;
    m_proxy->setSourceModel(model);
}

QAbstractItemModel* QtMaterialSearchView::sourceModel() const noexcept { return m_sourceModel; }
QtMaterialSearchBar* QtMaterialSearchView::searchBar() const noexcept { return m_searchBar; }
QListView* QtMaterialSearchView::resultView() const noexcept { return m_view; }
int QtMaterialSearchView::filterKeyColumn() const noexcept { return m_proxy->filterKeyColumn(); }
void QtMaterialSearchView::setFilterKeyColumn(int column) { m_proxy->setFilterKeyColumn(column); }
Qt::CaseSensitivity QtMaterialSearchView::filterCaseSensitivity() const noexcept { return m_proxy->filterCaseSensitivity(); }
void QtMaterialSearchView::setFilterCaseSensitivity(Qt::CaseSensitivity sensitivity) { m_proxy->setFilterCaseSensitivity(sensitivity); }

} // namespace QtMaterial
