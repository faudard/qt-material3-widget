#include "qtmaterial/widgets/data/qtmaterialpagination.h"

#include "qtmaterial/core/private/qtmaterialthemecontextbinding_p.h"
#include "../resolution/qtmaterialdataspecresolution_p.h"

#include <QComboBox>
#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QPalette>
#include <QSignalBlocker>
#include <QToolButton>

namespace QtMaterial {
namespace {

PaginationSpec normalizedPaginationSpec(PaginationSpec spec)
{
    spec.minimumHeight = qMax(32, spec.minimumHeight);
    spec.controlExtent = qMax(28, spec.controlExtent);
    spec.spacing = qMax(0, spec.spacing);
    spec.horizontalPadding = qMax(0, spec.horizontalPadding);
    spec.focusRingWidth = qMax(0, spec.focusRingWidth);
    return spec;
}

} // namespace

class QtMaterialPaginationPrivate final
{
public:
    int page = 1;
    int pageSize = 25;
    int totalCount = 0;

    PaginationSpec spec = defaultPaginationSpec();
    PaginationSpec explicitSpec = defaultPaginationSpec();
    bool specDirty = true;
    bool explicitSpecSet = false;

    QtMaterialThemeContextBinding* themeBinding = nullptr;
    QToolButton* firstButton = nullptr;
    QToolButton* previousButton = nullptr;
    QToolButton* nextButton = nullptr;
    QToolButton* lastButton = nullptr;
    QLabel* rangeLabel = nullptr;
    QComboBox* pageSizeCombo = nullptr;
};

QtMaterialPagination::QtMaterialPagination(QWidget* parent)
    : QWidget(parent)
    , d_ptr(std::make_unique<QtMaterialPaginationPrivate>())
{
    d_ptr->themeBinding = new QtMaterialThemeContextBinding(this, this);
    connect(
        d_ptr->themeBinding,
        &QtMaterialThemeContextBinding::effectiveThemeContextChanged,
        this,
        &QtMaterialPagination::effectiveThemeContextChanged);
    connect(
        d_ptr->themeBinding,
        &QtMaterialThemeContextBinding::themeChanged,
        this,
        [this](const Theme&) {
            if (d_ptr->explicitSpecSet) {
                return;
            }
            d_ptr->specDirty = true;
            ensureSpecResolved();
            applyResolvedSpec();
        });

    d_ptr->firstButton = new QToolButton(this);
    d_ptr->previousButton = new QToolButton(this);
    d_ptr->nextButton = new QToolButton(this);
    d_ptr->lastButton = new QToolButton(this);
    d_ptr->rangeLabel = new QLabel(this);
    d_ptr->pageSizeCombo = new QComboBox(this);

    setObjectName(QStringLiteral("QtMaterialPagination"));
    setAccessibleName(tr("Pagination"));

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addStretch(1);

    d_ptr->pageSizeCombo->setAccessibleName(tr("Rows per page"));
    layout->addWidget(d_ptr->pageSizeCombo);
    layout->addWidget(d_ptr->rangeLabel);

    d_ptr->firstButton->setAccessibleName(tr("First page"));
    d_ptr->previousButton->setAccessibleName(tr("Previous page"));
    d_ptr->nextButton->setAccessibleName(tr("Next page"));
    d_ptr->lastButton->setAccessibleName(tr("Last page"));

    layout->addWidget(d_ptr->firstButton);
    layout->addWidget(d_ptr->previousButton);
    layout->addWidget(d_ptr->nextButton);
    layout->addWidget(d_ptr->lastButton);

    setPageSizeOptions({10, 25, 50, 100});

    connect(d_ptr->firstButton, &QToolButton::clicked, this, [this]() { setPage(1); });
    connect(d_ptr->previousButton, &QToolButton::clicked, this, [this]() { setPage(d_ptr->page - 1); });
    connect(d_ptr->nextButton, &QToolButton::clicked, this, [this]() { setPage(d_ptr->page + 1); });
    connect(d_ptr->lastButton, &QToolButton::clicked, this, [this]() { setPage(pageCount()); });
    connect(
        d_ptr->pageSizeCombo,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        [this](int index) {
            if (index >= 0) {
                setPageSize(d_ptr->pageSizeCombo->itemData(index).toInt());
            }
        });

    ensureSpecResolved();
    applyResolvedSpec();
    updateUi();
}

QtMaterialPagination::~QtMaterialPagination() = default;

void QtMaterialPagination::setThemeContext(ThemeContext* context)
{
    if (d_ptr->themeBinding->themeContext() == context) {
        return;
    }
    d_ptr->themeBinding->setThemeContext(context);
    Q_EMIT themeContextChanged(context);
}

ThemeContext* QtMaterialPagination::themeContext() const noexcept
{
    return d_ptr->themeBinding->themeContext();
}

ThemeContext* QtMaterialPagination::effectiveThemeContext() const noexcept
{
    return d_ptr->themeBinding->effectiveThemeContext();
}

PaginationSpec QtMaterialPagination::spec() const
{
    return resolvedSpec();
}

const PaginationSpec& QtMaterialPagination::resolvedSpec() const
{
    ensureSpecResolved();
    return d_ptr->spec;
}

void QtMaterialPagination::setSpec(const PaginationSpec& spec)
{
    d_ptr->explicitSpec = normalizedPaginationSpec(spec);
    d_ptr->explicitSpecSet = true;
    d_ptr->specDirty = true;
    ensureSpecResolved();
    applyResolvedSpec();
}

void QtMaterialPagination::resetSpec()
{
    if (!d_ptr->explicitSpecSet) {
        return;
    }
    d_ptr->explicitSpecSet = false;
    d_ptr->specDirty = true;
    ensureSpecResolved();
    applyResolvedSpec();
}

bool QtMaterialPagination::hasExplicitSpec() const noexcept
{
    return d_ptr->explicitSpecSet;
}

int QtMaterialPagination::page() const noexcept { return d_ptr->page; }
int QtMaterialPagination::pageSize() const noexcept { return d_ptr->pageSize; }
int QtMaterialPagination::totalCount() const noexcept { return d_ptr->totalCount; }

int QtMaterialPagination::pageCount() const noexcept
{
    if (d_ptr->totalCount <= 0) {
        return 1;
    }
    return 1 + (d_ptr->totalCount - 1) / d_ptr->pageSize;
}

void QtMaterialPagination::setPage(int page)
{
    const int normalized = qBound(1, page, pageCount());
    if (d_ptr->page == normalized) {
        return;
    }
    d_ptr->page = normalized;
    updateUi();
    Q_EMIT pageChanged(d_ptr->page);
}

void QtMaterialPagination::setPageSize(int pageSize)
{
    if (pageSize <= 0 || d_ptr->pageSize == pageSize) {
        return;
    }

    d_ptr->pageSize = pageSize;
    const int normalizedPage = qBound(1, d_ptr->page, pageCount());
    const bool pageDidChange = normalizedPage != d_ptr->page;
    d_ptr->page = normalizedPage;

    int index = d_ptr->pageSizeCombo->findData(d_ptr->pageSize);
    if (index < 0) {
        d_ptr->pageSizeCombo->addItem(QString::number(d_ptr->pageSize), d_ptr->pageSize);
        index = d_ptr->pageSizeCombo->findData(d_ptr->pageSize);
    }
    {
        const QSignalBlocker blocker(d_ptr->pageSizeCombo);
        d_ptr->pageSizeCombo->setCurrentIndex(index);
    }

    updateUi();
    Q_EMIT pageSizeChanged(d_ptr->pageSize);
    if (pageDidChange) {
        Q_EMIT pageChanged(d_ptr->page);
    }
}

void QtMaterialPagination::setTotalCount(int totalCount)
{
    const int normalized = qMax(0, totalCount);
    if (d_ptr->totalCount == normalized) {
        return;
    }

    d_ptr->totalCount = normalized;
    const int normalizedPage = qBound(1, d_ptr->page, pageCount());
    const bool pageDidChange = normalizedPage != d_ptr->page;
    d_ptr->page = normalizedPage;
    updateUi();
    Q_EMIT totalCountChanged(d_ptr->totalCount);
    if (pageDidChange) {
        Q_EMIT pageChanged(d_ptr->page);
    }
}

QList<int> QtMaterialPagination::pageSizeOptions() const
{
    QList<int> result;
    for (int index = 0; index < d_ptr->pageSizeCombo->count(); ++index) {
        result.push_back(d_ptr->pageSizeCombo->itemData(index).toInt());
    }
    return result;
}

void QtMaterialPagination::setPageSizeOptions(const QList<int>& options)
{
    const QSignalBlocker blocker(d_ptr->pageSizeCombo);
    d_ptr->pageSizeCombo->clear();

    for (int value : options) {
        if (value > 0 && d_ptr->pageSizeCombo->findData(value) < 0) {
            d_ptr->pageSizeCombo->addItem(QString::number(value), value);
        }
    }

    int index = d_ptr->pageSizeCombo->findData(d_ptr->pageSize);
    if (index < 0) {
        d_ptr->pageSizeCombo->addItem(QString::number(d_ptr->pageSize), d_ptr->pageSize);
        index = d_ptr->pageSizeCombo->findData(d_ptr->pageSize);
    }
    d_ptr->pageSizeCombo->setCurrentIndex(index);
    updateUi();
}

QString QtMaterialPagination::rangeText() const
{
    if (d_ptr->totalCount == 0) {
        return QStringLiteral("0 / 0");
    }

    const qint64 first =
        qint64(d_ptr->page - 1) * qint64(d_ptr->pageSize) + 1;
    const qint64 last =
        qMin(qint64(d_ptr->totalCount), first + qint64(d_ptr->pageSize) - 1);
    return QStringLiteral("%1–%2 / %3")
        .arg(first)
        .arg(last)
        .arg(d_ptr->totalCount);
}

void QtMaterialPagination::ensureSpecResolved() const
{
    if (!d_ptr->specDirty) {
        return;
    }

    if (d_ptr->explicitSpecSet) {
        d_ptr->spec = d_ptr->explicitSpec;
    } else {
        d_ptr->spec =
            DataSpecResolution::paginationSpec(
                d_ptr->themeBinding,
                Density::Default);
    }
    d_ptr->specDirty = false;
}

void QtMaterialPagination::applyResolvedSpec()
{
    ensureSpecResolved();
    const PaginationSpec& current = d_ptr->spec;

    setMinimumHeight(current.minimumHeight);
    setFont(current.labelFont);

    if (auto* row = qobject_cast<QHBoxLayout*>(layout())) {
        row->setContentsMargins(
            current.horizontalPadding,
            0,
            current.horizontalPadding,
            0);
        row->setSpacing(current.spacing);
    }

    QPalette resolved = palette();
    resolved.setColor(QPalette::Window, current.backgroundColor);
    resolved.setColor(QPalette::WindowText, current.foregroundColor);
    resolved.setColor(QPalette::Text, current.foregroundColor);
    resolved.setColor(QPalette::ButtonText, current.controlColor);
    resolved.setColor(QPalette::Highlight, current.focusRingColor);
    resolved.setColor(
        QPalette::Disabled,
        QPalette::ButtonText,
        current.disabledControlColor);
    resolved.setColor(
        QPalette::Disabled,
        QPalette::Text,
        current.disabledControlColor);
    setPalette(resolved);

    d_ptr->rangeLabel->setFont(current.labelFont);
    d_ptr->rangeLabel->setPalette(resolved);
    d_ptr->pageSizeCombo->setFont(current.labelFont);
    d_ptr->pageSizeCombo->setPalette(resolved);
    d_ptr->pageSizeCombo->setMinimumHeight(current.controlExtent);

    const QList<QToolButton*> controls = {
        d_ptr->firstButton,
        d_ptr->previousButton,
        d_ptr->nextButton,
        d_ptr->lastButton
    };
    for (QToolButton* button : controls) {
        button->setFont(current.labelFont);
        button->setPalette(resolved);
        button->setFixedSize(
            current.controlExtent,
            current.controlExtent);
    }

    updateGeometry();
}

void QtMaterialPagination::updateUi()
{
    const bool hasPrevious = d_ptr->page > 1;
    const bool hasNext = d_ptr->page < pageCount();
    const bool rtl = layoutDirection() == Qt::RightToLeft;

    d_ptr->firstButton->setText(
        rtl ? QStringLiteral("»") : QStringLiteral("«"));
    d_ptr->previousButton->setText(
        rtl ? QStringLiteral("›") : QStringLiteral("‹"));
    d_ptr->nextButton->setText(
        rtl ? QStringLiteral("‹") : QStringLiteral("›"));
    d_ptr->lastButton->setText(
        rtl ? QStringLiteral("«") : QStringLiteral("»"));

    d_ptr->firstButton->setEnabled(hasPrevious);
    d_ptr->previousButton->setEnabled(hasPrevious);
    d_ptr->nextButton->setEnabled(hasNext);
    d_ptr->lastButton->setEnabled(hasNext);
    d_ptr->rangeLabel->setText(rangeText());
    d_ptr->rangeLabel->setAccessibleName(tr("Items %1").arg(rangeText()));

    setAccessibleDescription(
        tr("Page %1 of %2, items %3")
            .arg(d_ptr->page)
            .arg(pageCount())
            .arg(rangeText()));
}

void QtMaterialPagination::changeEvent(QEvent* event)
{
    QWidget::changeEvent(event);
    if (event
        && (event->type() == QEvent::LayoutDirectionChange
            || event->type() == QEvent::LanguageChange)) {
        updateUi();
    }
}

} // namespace QtMaterial
