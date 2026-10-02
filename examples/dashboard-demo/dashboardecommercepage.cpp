#include "dashboardecommercepage.h"

#include "ui_dashboardecommercepage.h"

#include <QAbstractButton>
#include <QApplication>
#include <QColor>
#include <QFont>
#include <QGridLayout>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QShowEvent>
#include <QPainter>
#include <QPainterPath>
#include <QStandardItemModel>
#include <QSizePolicy>
#include <QStyle>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QVBoxLayout>

#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"
#include "qtmaterial/widgets/data/qtmaterialtable.h"
#include "qtmaterial/widgets/inputs/qtmaterialcombobox.h"
#include "qtmaterial/widgets/selection/qtmaterialchip.h"
#include "qtmaterial/widgets/selection/qtmaterialsegmentedbutton.h"
#include "qtmaterial/widgets/surfaces/qtmaterialcard.h"

namespace {

QColor color(QtMaterial::ColorRole role)
{
    return QtMaterial::ThemeManager::instance().theme().colorScheme().color(role);
}

QString cssColor(const QColor& value)
{
    return value.name(QColor::HexRgb);
}

QLabel* label(
    const QString& text,
    QWidget* parent,
    qreal delta = 0.0,
    bool bold = false)
{
    auto* result = new QLabel(text, parent);
    QFont font = result->font();
    font.setPointSizeF(qMax<qreal>(8.0, font.pointSizeF() + delta));
    font.setBold(bold);
    result->setFont(font);
    result->setWordWrap(true);
    return result;
}

class InstalledAreaChart final : public QWidget
{
public:
    explicit InstalledAreaChart(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        setMinimumHeight(270);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        connect(
            &QtMaterial::ThemeManager::instance(),
            &QtMaterial::ThemeManager::themeChanged,
            this,
            [this](const QtMaterial::Theme&) { update(); });
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        const QRectF plot = rect().adjusted(38.0, 18.0, -18.0, -36.0);
        if (plot.width() < 80.0 || plot.height() < 80.0) {
            return;
        }

        QColor grid = color(QtMaterial::ColorRole::OutlineVariant);
        grid.setAlpha(120);
        painter.setPen(QPen(grid, 1.0, Qt::DashLine));
        for (int i = 0; i <= 4; ++i) {
            const qreal y = plot.bottom() - plot.height() * i / 4.0;
            painter.drawLine(QPointF(plot.left(), y), QPointF(plot.right(), y));
        }

        static const int asia[] = {6,18,14,8,20,6,22,19,8,22,8,17};
        static const int europe[] = {6,18,14,10,20,6,22,19,8,22,8,18};
        static const int americas[] = {6,18,14,10,20,6,22,19,8,22,8,16};

        const int count = 12;
        const qreal slot = plot.width() / count;
        const qreal barWidth = qMin<qreal>(28.0, slot * 0.62);
        const qreal scale = plot.height() / 80.0;

        for (int i = 0; i < count; ++i) {
            const qreal x = plot.left() + slot * i + (slot - barWidth) / 2.0;
            qreal bottom = plot.bottom();

            const struct {
                int value;
                QtMaterial::ColorRole role;
            } segments[] = {
                {asia[i], QtMaterial::ColorRole::Primary},
                {europe[i], QtMaterial::ColorRole::Tertiary},
                {americas[i], QtMaterial::ColorRole::Secondary}
            };

            for (const auto& segment : segments) {
                const qreal height = segment.value * scale;
                QRectF r(x, bottom - height, barWidth, height);
                painter.setPen(Qt::NoPen);
                painter.setBrush(color(segment.role));
                painter.drawRoundedRect(r, 3.0, 3.0);
                bottom -= height;
            }
        }

        static const char* months[] = {
            "Jan","Feb","Mar","Apr","May","Jun",
            "Jul","Aug","Sep","Oct","Nov","Dec"
        };
        painter.setPen(color(QtMaterial::ColorRole::OnSurfaceVariant));
        QFont monthFont = font();
        monthFont.setPointSizeF(qMax<qreal>(8.0, monthFont.pointSizeF() - 1.0));
        painter.setFont(monthFont);
        for (int i = 0; i < count; ++i) {
            const QRectF textRect(
                plot.left() + slot * i,
                plot.bottom() + 6.0,
                slot,
                22.0);
            painter.drawText(
                textRect,
                Qt::AlignHCenter | Qt::AlignTop,
                QString::fromLatin1(months[i]));
        }
    }
};

class InvoiceStatusDelegate final : public QStyledItemDelegate
{
public:
    explicit InvoiceStatusDelegate(QObject* parent = nullptr)
        : QStyledItemDelegate(parent)
    {
    }

    void paint(
        QPainter* painter,
        const QStyleOptionViewItem& option,
        const QModelIndex& index) const override
    {
        QStyleOptionViewItem base(option);
        initStyleOption(&base, index);
        const QString text = base.text;
        base.text.clear();

        QStyle* style =
            option.widget
                ? option.widget->style()
                : QApplication::style();
        style->drawControl(
            QStyle::CE_ItemViewItem,
            &base,
            painter,
            option.widget);

        const bool dark =
            QtMaterial::ThemeManager::instance().theme().isDark();

        QColor foreground;
        QColor background;
        if (text == QStringLiteral("Paid")) {
            foreground = QColor(dark ? QStringLiteral("#86EFAC") : QStringLiteral("#118D57"));
            background = QColor(dark ? QStringLiteral("#163B2B") : QStringLiteral("#D8FBDE"));
        } else if (text == QStringLiteral("Out of date")) {
            foreground = QColor(dark ? QStringLiteral("#FFB4AB") : QStringLiteral("#B42318"));
            background = QColor(dark ? QStringLiteral("#4B1D1A") : QStringLiteral("#FFE9E7"));
        } else {
            foreground = QColor(dark ? QStringLiteral("#FFD18B") : QStringLiteral("#B76E00"));
            background = QColor(dark ? QStringLiteral("#493416") : QStringLiteral("#FFF2D8"));
        }

        QFont font = option.font;
        font.setBold(true);
        font.setPointSizeF(qMax<qreal>(8.0, font.pointSizeF() - 1.0));
        painter->setFont(font);

        const QFontMetrics metrics(font);
        const int pillWidth =
            qMin(metrics.horizontalAdvance(text) + 16, option.rect.width() - 24);
        const QRect pill(
            option.rect.left() + 12,
            option.rect.center().y() - 13,
            qMax(24, pillWidth),
            26);

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(Qt::NoPen);
        painter->setBrush(background);
        painter->drawRoundedRect(pill, 6.0, 6.0);
        painter->setPen(foreground);
        painter->drawText(pill, Qt::AlignCenter, text);
        painter->restore();
    }
};

QtMaterial::QtMaterialCard* makeCard(
    const QString& title,
    QWidget* parent,
    int minHeight = 260)
{
    auto* card = new QtMaterial::QtMaterialCard(parent);
    card->setVariant(QtMaterial::QtMaterialCard::Variant::Outlined);
    card->setMinimumHeight(minHeight);

    auto* layout = new QVBoxLayout(card);
    layout->setContentsMargins(20, 18, 20, 18);
    layout->setSpacing(12);
    layout->addWidget(label(title, card, 3.0, true));
    return card;
}

} // namespace

DashboardEcommercePage::DashboardEcommercePage(QWidget* parent)
    : QWidget(parent)
    , m_ui(new Ui::DashboardEcommercePage)
{
    m_ui->setupUi(this);
    setObjectName(QStringLiteral("dashboardContent"));

    QFont titleFont = m_ui->titleLabel->font();
    titleFont.setPointSizeF(titleFont.pointSizeF() + 5.0);
    titleFont.setBold(true);
    m_ui->titleLabel->setFont(titleFont);
    m_ui->subtitleLabel->setObjectName(QStringLiteral("pageSubtitle"));

    auto* applications = makeCard(
        QStringLiteral("Related applications"),
        this,
        420);
    auto* applicationsLayout =
        static_cast<QVBoxLayout*>(applications->layout());

    auto* period = new QtMaterial::QtMaterialSegmentedButton(applications);
    period->addSegment(QStringLiteral("Top 7 days"));
    period->addSegment(QStringLiteral("Top 30 days"));
    period->addSegment(QStringLiteral("All times"));
    period->setCurrentIndex(0);
    period->setMinimumWidth(period->sizeHint().width());
    applicationsLayout->addWidget(period, 0, Qt::AlignLeft);

    const struct {
        const char* symbol;
        const char* name;
        const char* price;
        const char* downloads;
        const char* size;
        const char* rating;
    } appData[] = {
        {"M", "Microsoft office 365", "Free", "9.91k", "9.68 Mb", "9.91k"},
        {"O", "Opera", "Free", "1.95k", "1.9 Mb", "1.95k"},
        {"A", "Adobe acrobat reader DC", "€68.71", "9.12k", "8.91 Mb", "9.12k"},
        {"J", "Joplin", "Free", "6.98k", "6.82 Mb", "6.98k"},
        {"T", "Topaz photo AI", "€52.17", "8.49k", "8.29 Mb", "8.49k"}
    };

    for (const auto& app : appData) {
        auto* row = new QWidget(applications);
        row->setObjectName(QStringLiteral("ecommerceAppRow"));
        row->setMinimumHeight(58);
        auto* rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(6, 4, 6, 4);
        rowLayout->setSpacing(12);

        auto* icon = new QLabel(QString::fromLatin1(app.symbol), row);
        icon->setObjectName(QStringLiteral("ecommerceAppIcon"));
        icon->setAlignment(Qt::AlignCenter);
        icon->setFixedSize(46, 46);
        rowLayout->addWidget(icon);

        auto* copy = new QVBoxLayout;
        copy->setSpacing(3);

        auto* titleRow = new QHBoxLayout;
        titleRow->addWidget(label(
            QString::fromLatin1(app.name),
            row,
            1.0,
            true));
        auto* price = new QtMaterial::QtMaterialChip(
            QString::fromUtf8(app.price),
            row);
        price->setVariant(QtMaterial::ChipVariant::Assist);
        titleRow->addWidget(price);
        titleRow->addStretch(1);
        copy->addLayout(titleRow);

        copy->addWidget(label(
            QStringLiteral("↓ %1   •   ▣ %2   •   ★ %3")
                .arg(QString::fromLatin1(app.downloads))
                .arg(QString::fromLatin1(app.size))
                .arg(QString::fromLatin1(app.rating)),
            row,
            -1.0,
            false));

        rowLayout->addLayout(copy, 1);
        applicationsLayout->addWidget(row);
    }

    auto* area = makeCard(QStringLiteral("Area installed"), this, 420);
    auto* areaLayout = static_cast<QVBoxLayout*>(area->layout());

    auto* areaHeader = new QHBoxLayout;
    auto* trendCopy = new QVBoxLayout;
    trendCopy->setSpacing(2);
    trendCopy->addWidget(label(
        QStringLiteral("(+43%) than last year"),
        area,
        0.0,
        false));

    auto* legend = new QHBoxLayout;
    legend->setSpacing(14);
    legend->addWidget(label(QStringLiteral("● Asia 1.23k"), area, -1.0, false));
    legend->addWidget(label(QStringLiteral("● Europe 6.79k"), area, -1.0, false));
    legend->addStretch(1);
    trendCopy->addLayout(legend);
    areaHeader->addLayout(trendCopy, 1);

    auto* year = new QtMaterial::QtMaterialComboBox(area);
    year->addItems({
        QStringLiteral("2022"),
        QStringLiteral("2023"),
        QStringLiteral("2024")
    });
    year->setCurrentText(QStringLiteral("2023"));
    areaHeader->addWidget(year, 0, Qt::AlignTop);
    areaLayout->addLayout(areaHeader);

    auto* chart = new InstalledAreaChart(area);
    areaLayout->addWidget(chart, 1);

    auto* invoices = makeCard(QStringLiteral("New Invoices"), this, 470);
    auto* invoicesLayout = static_cast<QVBoxLayout*>(invoices->layout());
    invoicesLayout->setContentsMargins(0, 0, 0, 12);
    invoicesLayout->setSpacing(0);
    if (auto* invoiceTitle =
            qobject_cast<QLabel*>(invoicesLayout->itemAt(0)->widget())) {
        invoiceTitle->setContentsMargins(20, 18, 20, 14);
        invoiceTitle->setMinimumHeight(70);
    }

    auto* table = new QtMaterial::QtMaterialTable(invoices);
    table->setDense(false);
    table->setAlternatingRowColors(false);
    table->verticalHeader()->setVisible(false);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setSortingEnabled(false);
    table->horizontalHeader()->setSectionsClickable(false);

    auto* model = new QStandardItemModel(5, 5, table);
    model->setHorizontalHeaderLabels({
        QStringLiteral("Invoice ID"),
        QStringLiteral("Category"),
        QStringLiteral("Price"),
        QStringLiteral("Status"),
        QString()
    });

    const char* rows[][4] = {
        {"INV-1990", "Android", "$83.74", "Paid"},
        {"INV-1991", "Mac", "$97.14", "Out of date"},
        {"INV-1992", "Windows", "$68.71", "Progress"},
        {"INV-1993", "Android", "$85.21", "Paid"},
        {"INV-1994", "Mac", "$52.17", "Paid"}
    };
    for (int r = 0; r < 5; ++r) {
        for (int col = 0; col < 4; ++col) {
            model->setItem(
                r,
                col,
                new QStandardItem(QString::fromLatin1(rows[r][col])));
        }

        auto* action = new QStandardItem(QStringLiteral("⋮"));
        action->setTextAlignment(Qt::AlignCenter);
        action->setData(
            QStringLiteral("More actions for %1")
                .arg(QString::fromLatin1(rows[r][0])),
            Qt::AccessibleTextRole);
        model->setItem(r, 4, action);
    }

    table->setModel(model);
    table->setItemDelegateForColumn(3, new InvoiceStatusDelegate(table));
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
    table->horizontalHeader()->resizeSection(4, 48);
    invoicesLayout->addWidget(table, 1);

    auto* footer = new QWidget(invoices);
    auto* footerLayout = new QHBoxLayout(footer);
    footerLayout->setContentsMargins(20, 8, 16, 0);
    footerLayout->setSpacing(0);
    footerLayout->addStretch(1);

    auto* viewAll = new QtMaterial::QtMaterialTextButton(
        QStringLiteral("View all  ›"),
        footer);
    footerLayout->addWidget(viewAll);
    invoicesLayout->addWidget(footer);

    auto* countries = makeCard(
        QStringLiteral("Top installed countries"),
        this,
        300);
    auto* countriesLayout =
        static_cast<QVBoxLayout*>(countries->layout());

    const QStringList countryRows = {
        QStringLiteral("🇩🇪  Germany                         Android 9.91k      Windows 1.95k"),
        QStringLiteral("🇫🇷  France                           Android 8.42k      Windows 1.72k"),
        QStringLiteral("🇬🇧  United Kingdom           Android 7.98k      Windows 1.63k"),
        QStringLiteral("🇺🇸  United States               Android 7.45k      Windows 1.58k")
    };
    for (const QString& row : countryRows) {
        countriesLayout->addWidget(label(row, countries, 0.0, false));
    }
    countriesLayout->addStretch(1);

    m_ui->contentGrid->addWidget(applications, 0, 0);
    m_ui->contentGrid->addWidget(area, 0, 1);
    m_ui->contentGrid->addWidget(invoices, 1, 0);
    m_ui->contentGrid->addWidget(countries, 1, 1);
    m_ui->contentGrid->setColumnStretch(0, 1);
    m_ui->contentGrid->setColumnStretch(1, 1);

    connect(period, &QtMaterial::QtMaterialSegmentedButton::currentIndexChanged, this, [this](int index) {
        const char* labels[] = {"Top 7 days", "Top 30 days", "All times"};
        emit messageRequested(
            QStringLiteral("Application ranking switched to %1.")
                .arg(QString::fromLatin1(labels[qBound(0, index, 2)])));
    });
    connect(year, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) {
        emit messageRequested(QStringLiteral("Installed-area period updated."));
    });
    connect(viewAll, &QAbstractButton::clicked, this, [this]() {
        emit messageRequested(QStringLiteral("Invoice history opened."));
    });

    connect(
        &QtMaterial::ThemeManager::instance(),
        &QtMaterial::ThemeManager::themeChanged,
        this,
        [this](const QtMaterial::Theme&) {
            if (isVisible()) {
                applyTheme();
            }
        });

    applyTheme();
}

void DashboardEcommercePage::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    applyTheme();
}

DashboardEcommercePage::~DashboardEcommercePage()
{
    delete m_ui;
}

void DashboardEcommercePage::applyTheme()
{
    const QColor surface = color(QtMaterial::ColorRole::Surface);
    const QColor surfaceLow = color(QtMaterial::ColorRole::SurfaceContainerLow);
    const QColor onSurface = color(QtMaterial::ColorRole::OnSurface);
    const QColor onSurfaceVariant = color(QtMaterial::ColorRole::OnSurfaceVariant);
    const QColor primaryContainer = color(QtMaterial::ColorRole::PrimaryContainer);
    const QColor onPrimaryContainer = color(QtMaterial::ColorRole::OnPrimaryContainer);
    const QColor outline = color(QtMaterial::ColorRole::OutlineVariant);

    setStyleSheet(QStringLiteral(
        "#dashboardContent { background:%1; }"
        "#dashboardContent QLabel { color:%2; }"
        "#dashboardContent QLabel#pageSubtitle { color:%3; }"
        "#dashboardContent #ecommerceAppRow { background:%4; border:1px solid %5; border-radius:12px; }"
        "#dashboardContent #ecommerceAppIcon { background:%6; color:%7; border-radius:12px; font-weight:700; font-size:17px; }")
        .arg(cssColor(surface))
        .arg(cssColor(onSurface))
        .arg(cssColor(onSurfaceVariant))
        .arg(cssColor(surfaceLow))
        .arg(cssColor(outline))
        .arg(cssColor(primaryContainer))
        .arg(cssColor(onPrimaryContainer)));
}
