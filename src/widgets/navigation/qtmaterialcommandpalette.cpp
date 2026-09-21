#include "qtmaterial/widgets/navigation/qtmaterialcommandpalette.h"

#include <QAbstractItemModel>
#include <QLineEdit>
#include <QListView>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>

namespace QtMaterial {

QtMaterialCommandPalette::QtMaterialCommandPalette(QWidget* parent)
    : QDialog(parent)
    , m_searchEdit(new QLineEdit(this))
    , m_resultView(new QListView(this))
    , m_proxyModel(new QSortFilterProxyModel(this))
{
    setObjectName(QStringLiteral("QtMaterialCommandPalette"));
    setAccessibleName(tr("Command palette"));
    setWindowTitle(tr("Commands"));
    setModal(true);
    resize(560, 400);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(8);

    m_searchEdit->setPlaceholderText(tr("Search commands"));
    m_searchEdit->setAccessibleName(tr("Search commands"));
    layout->addWidget(m_searchEdit);

    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxyModel->setFilterKeyColumn(-1);
    m_resultView->setModel(m_proxyModel);
    m_resultView->setSelectionMode(QAbstractItemView::SingleSelection);
    layout->addWidget(m_resultView, 1);

    connect(m_searchEdit, &QLineEdit::textChanged, this, [this](const QString& text) {
        m_proxyModel->setFilterFixedString(text);
        Q_EMIT queryChanged(text);
    });
    connect(m_resultView, &QListView::activated, this, &QtMaterialCommandPalette::activateProxyIndex);
    connect(m_resultView, &QListView::doubleClicked, this, &QtMaterialCommandPalette::activateProxyIndex);
    connect(m_searchEdit, &QLineEdit::returnPressed, this, [this]() {
        QModelIndex proxyIndex = m_resultView->currentIndex();
        if (!proxyIndex.isValid() && m_proxyModel->rowCount() > 0) {
            proxyIndex = m_proxyModel->index(0, 0);
        }
        activateProxyIndex(proxyIndex);
    });
}

void QtMaterialCommandPalette::setSourceModel(QAbstractItemModel* model)
{
    m_proxyModel->setSourceModel(model);
}

QAbstractItemModel* QtMaterialCommandPalette::sourceModel() const
{
    return m_proxyModel->sourceModel();
}

QString QtMaterialCommandPalette::query() const
{
    return m_searchEdit->text();
}

void QtMaterialCommandPalette::setQuery(const QString& query)
{
    m_searchEdit->setText(query);
}

QLineEdit* QtMaterialCommandPalette::searchEdit() const noexcept
{
    return m_searchEdit;
}

QListView* QtMaterialCommandPalette::resultView() const noexcept
{
    return m_resultView;
}

void QtMaterialCommandPalette::activateProxyIndex(const QModelIndex& proxyIndex)
{
    if (!proxyIndex.isValid()) {
        return;
    }

    const QModelIndex sourceIndex = m_proxyModel->mapToSource(proxyIndex);
    if (!sourceIndex.isValid()) {
        return;
    }

    Q_EMIT commandActivated(sourceIndex);
}

} // namespace QtMaterial
