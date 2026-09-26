#include "qtmaterial/widgets/data/qtmaterialpagination.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSignalBlocker>
#include <QToolButton>

namespace QtMaterial {

QtMaterialPagination::QtMaterialPagination(QWidget* parent)
    : QWidget(parent)
    , m_firstButton(new QToolButton(this))
    , m_previousButton(new QToolButton(this))
    , m_nextButton(new QToolButton(this))
    , m_lastButton(new QToolButton(this))
    , m_rangeLabel(new QLabel(this))
    , m_pageSizeCombo(new QComboBox(this))
{
    setObjectName(QStringLiteral("QtMaterialPagination"));
    setAccessibleName(tr("Pagination"));

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);
    layout->addStretch(1);

    m_pageSizeCombo->setAccessibleName(tr("Rows per page"));
    layout->addWidget(m_pageSizeCombo);
    layout->addWidget(m_rangeLabel);

    m_firstButton->setText(QStringLiteral("«"));
    m_previousButton->setText(QStringLiteral("‹"));
    m_nextButton->setText(QStringLiteral("›"));
    m_lastButton->setText(QStringLiteral("»"));

    m_firstButton->setAccessibleName(tr("First page"));
    m_previousButton->setAccessibleName(tr("Previous page"));
    m_nextButton->setAccessibleName(tr("Next page"));
    m_lastButton->setAccessibleName(tr("Last page"));

    layout->addWidget(m_firstButton);
    layout->addWidget(m_previousButton);
    layout->addWidget(m_nextButton);
    layout->addWidget(m_lastButton);

    setPageSizeOptions({10, 25, 50, 100});

    connect(m_firstButton, &QToolButton::clicked, this, [this]() { setPage(1); });
    connect(m_previousButton, &QToolButton::clicked, this, [this]() { setPage(m_page - 1); });
    connect(m_nextButton, &QToolButton::clicked, this, [this]() { setPage(m_page + 1); });
    connect(m_lastButton, &QToolButton::clicked, this, [this]() { setPage(pageCount()); });
    connect(
        m_pageSizeCombo,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        [this](int index) {
            if (index >= 0) {
                setPageSize(m_pageSizeCombo->itemData(index).toInt());
            }
        });

    updateUi();
}

int QtMaterialPagination::page() const noexcept { return m_page; }
int QtMaterialPagination::pageSize() const noexcept { return m_pageSize; }
int QtMaterialPagination::totalCount() const noexcept { return m_totalCount; }

int QtMaterialPagination::pageCount() const noexcept
{
    if (m_totalCount <= 0) {
        return 1;
    }
    return 1 + (m_totalCount - 1) / m_pageSize;
}

void QtMaterialPagination::setPage(int page)
{
    const int normalized = qBound(1, page, pageCount());
    if (m_page == normalized) {
        return;
    }
    m_page = normalized;
    updateUi();
    Q_EMIT pageChanged(m_page);
}

void QtMaterialPagination::setPageSize(int pageSize)
{
    if (pageSize <= 0 || m_pageSize == pageSize) {
        return;
    }

    m_pageSize = pageSize;
    const int normalizedPage = qBound(1, m_page, pageCount());
    const bool pageDidChange = normalizedPage != m_page;
    m_page = normalizedPage;

    int index = m_pageSizeCombo->findData(m_pageSize);
    if (index < 0) {
        m_pageSizeCombo->addItem(QString::number(m_pageSize), m_pageSize);
        index = m_pageSizeCombo->findData(m_pageSize);
    }
    {
        const QSignalBlocker blocker(m_pageSizeCombo);
        m_pageSizeCombo->setCurrentIndex(index);
    }

    updateUi();
    Q_EMIT pageSizeChanged(m_pageSize);
    if (pageDidChange) {
        Q_EMIT pageChanged(m_page);
    }
}

void QtMaterialPagination::setTotalCount(int totalCount)
{
    const int normalized = qMax(0, totalCount);
    if (m_totalCount == normalized) {
        return;
    }

    m_totalCount = normalized;
    const int normalizedPage = qBound(1, m_page, pageCount());
    const bool pageDidChange = normalizedPage != m_page;
    m_page = normalizedPage;
    updateUi();
    Q_EMIT totalCountChanged(m_totalCount);
    if (pageDidChange) {
        Q_EMIT pageChanged(m_page);
    }
}

QList<int> QtMaterialPagination::pageSizeOptions() const
{
    QList<int> result;
    for (int index = 0; index < m_pageSizeCombo->count(); ++index) {
        result.push_back(m_pageSizeCombo->itemData(index).toInt());
    }
    return result;
}

void QtMaterialPagination::setPageSizeOptions(const QList<int>& options)
{
    const QSignalBlocker blocker(m_pageSizeCombo);
    m_pageSizeCombo->clear();

    for (int value : options) {
        if (value > 0 && m_pageSizeCombo->findData(value) < 0) {
            m_pageSizeCombo->addItem(QString::number(value), value);
        }
    }

    int index = m_pageSizeCombo->findData(m_pageSize);
    if (index < 0) {
        m_pageSizeCombo->addItem(QString::number(m_pageSize), m_pageSize);
        index = m_pageSizeCombo->findData(m_pageSize);
    }
    m_pageSizeCombo->setCurrentIndex(index);
    updateUi();
}

QString QtMaterialPagination::rangeText() const
{
    if (m_totalCount == 0) {
        return QStringLiteral("0 / 0");
    }

    const int first = (m_page - 1) * m_pageSize + 1;
    const int last = qMin(m_totalCount, first + m_pageSize - 1);
    return QStringLiteral("%1–%2 / %3").arg(first).arg(last).arg(m_totalCount);
}

void QtMaterialPagination::updateUi()
{
    const bool hasPrevious = m_page > 1;
    const bool hasNext = m_page < pageCount();

    m_firstButton->setEnabled(hasPrevious);
    m_previousButton->setEnabled(hasPrevious);
    m_nextButton->setEnabled(hasNext);
    m_lastButton->setEnabled(hasNext);
    m_rangeLabel->setText(rangeText());
    m_rangeLabel->setAccessibleName(tr("Items %1").arg(rangeText()));
}

} // namespace QtMaterial
