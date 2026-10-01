#include "dashboardinvoicepage.h"

#include "ui_dashboardinvoicepage.h"

#include <QAbstractButton>
#include <QColor>
#include <QFont>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QStandardItemModel>
#include <QVBoxLayout>

#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h"
#include "qtmaterial/widgets/data/qtmaterialtable.h"
#include "qtmaterial/widgets/selection/qtmaterialchip.h"
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

QLabel* makeLabel(
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

QtMaterial::QtMaterialCard* makeCard(
    const QString& title,
    QWidget* parent,
    int minimumHeight = 180)
{
    auto* card = new QtMaterial::QtMaterialCard(parent);
    card->setVariant(QtMaterial::QtMaterialCard::Variant::Outlined);
    card->setMinimumHeight(minimumHeight);

    auto* layout = new QVBoxLayout(card);
    layout->setContentsMargins(20, 18, 20, 18);
    layout->setSpacing(10);
    layout->addWidget(makeLabel(title, card, 2.0, true));
    return card;
}

} // namespace

DashboardInvoicePage::DashboardInvoicePage(QWidget* parent)
    : QWidget(parent)
    , m_ui(new Ui::DashboardInvoicePage)
{
    m_ui->setupUi(this);
    setObjectName(QStringLiteral("dashboardContent"));

    auto* heading = new QVBoxLayout;
    heading->setSpacing(2);

    auto* title = makeLabel(QStringLiteral("Invoice #INV-1994"), this, 5.0, true);
    title->setObjectName(QStringLiteral("invoiceTitle"));
    heading->addWidget(title);

    auto* subtitle = makeLabel(
        QStringLiteral("Issued 24 Sep 2026 · Due 08 Oct 2026"),
        this,
        -1.0,
        false);
    subtitle->setObjectName(QStringLiteral("pageSubtitle"));
    heading->addWidget(subtitle);

    m_ui->headerLayout->addLayout(heading, 1);

    auto* status = new QtMaterial::QtMaterialChip(
        QStringLiteral("Pending"),
        this);
    status->setVariant(QtMaterial::ChipVariant::Assist);
    status->setObjectName(QStringLiteral("invoiceStatusChip"));
    m_ui->headerLayout->addWidget(status, 0, Qt::AlignTop);

    auto* download = new QtMaterial::QtMaterialOutlinedButton(
        QStringLiteral("Download PDF"),
        this);
    auto* pay = new QtMaterial::QtMaterialFilledButton(
        QStringLiteral("Mark as paid"),
        this);
    m_ui->headerLayout->addWidget(download, 0, Qt::AlignTop);
    m_ui->headerLayout->addWidget(pay, 0, Qt::AlignTop);

    auto* billedTo = makeCard(QStringLiteral("Billed to"), this);
    auto* billedLayout = static_cast<QVBoxLayout*>(billedTo->layout());
    billedLayout->addWidget(makeLabel(QStringLiteral("John Doe"), billedTo, 1.0, true));
    billedLayout->addWidget(makeLabel(QStringLiteral("Material Labs"), billedTo));
    billedLayout->addWidget(makeLabel(QStringLiteral("12 Avenue des Widgets"), billedTo));
    billedLayout->addWidget(makeLabel(QStringLiteral("31000 Toulouse · France"), billedTo));
    billedLayout->addWidget(makeLabel(QStringLiteral("john.doe@example.com"), billedTo));
    billedLayout->addStretch(1);

    auto* payment = makeCard(QStringLiteral("Payment"), this);
    auto* paymentLayout = static_cast<QVBoxLayout*>(payment->layout());
    paymentLayout->addWidget(makeLabel(QStringLiteral("Visa •••• 2048"), payment, 1.0, true));
    paymentLayout->addWidget(makeLabel(QStringLiteral("Payment terms · 14 days"), payment));
    paymentLayout->addWidget(makeLabel(QStringLiteral("Currency · EUR"), payment));
    paymentLayout->addWidget(makeLabel(QStringLiteral("Reference · PMT-2026-1994"), payment));
    paymentLayout->addStretch(1);

    auto* total = makeCard(QStringLiteral("Amount due"), this);
    auto* totalLayout = static_cast<QVBoxLayout*>(total->layout());
    totalLayout->addWidget(makeLabel(QStringLiteral("€59.00"), total, 8.0, true));
    totalLayout->addWidget(makeLabel(QStringLiteral("Includes €9.83 VAT"), total));
    totalLayout->addStretch(1);
    auto* reminder = new QtMaterial::QtMaterialOutlinedButton(
        QStringLiteral("Send reminder"),
        total);
    totalLayout->addWidget(reminder, 0, Qt::AlignLeft);

    m_ui->summaryGrid->addWidget(billedTo, 0, 0);
    m_ui->summaryGrid->addWidget(payment, 0, 1);
    m_ui->summaryGrid->addWidget(total, 0, 2);
    m_ui->summaryGrid->setColumnStretch(0, 1);
    m_ui->summaryGrid->setColumnStretch(1, 1);
    m_ui->summaryGrid->setColumnStretch(2, 1);

    auto* invoiceCard = makeCard(QStringLiteral("Invoice items"), this, 430);
    auto* invoiceCardLayout = static_cast<QVBoxLayout*>(invoiceCard->layout());

    auto* table = new QtMaterial::QtMaterialTable(invoiceCard);
    table->setDense(false);
    table->setAlternatingRowColors(false);
    table->verticalHeader()->setVisible(false);
    table->setSelectionMode(QAbstractItemView::NoSelection);

    auto* model = new QStandardItemModel(4, 5, table);
    model->setHorizontalHeaderLabels({
        QStringLiteral("Description"),
        QStringLiteral("Qty"),
        QStringLiteral("Unit price"),
        QStringLiteral("VAT"),
        QStringLiteral("Total")
    });

    const char* rows[][5] = {
        {"Professional plan", "1", "€29.00", "20%", "€29.00"},
        {"Priority onboarding", "1", "€18.00", "20%", "€18.00"},
        {"Storage add-on", "2", "€4.00", "20%", "€8.00"},
        {"Support credits", "1", "€4.00", "20%", "€4.00"}
    };

    for (int row = 0; row < 4; ++row) {
        for (int column = 0; column < 5; ++column) {
            model->setItem(
                row,
                column,
                new QStandardItem(QString::fromUtf8(rows[row][column])));
        }
    }

    table->setModel(model);
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    for (int column = 1; column < 5; ++column) {
        table->horizontalHeader()->setSectionResizeMode(
            column,
            QHeaderView::ResizeToContents);
    }
    invoiceCardLayout->addWidget(table, 1);

    auto* totals = new QGridLayout;
    totals->setHorizontalSpacing(18);
    totals->setVerticalSpacing(6);
    totals->setColumnStretch(0, 1);

    const QStringList labels = {
        QStringLiteral("Subtotal"),
        QStringLiteral("VAT"),
        QStringLiteral("Total")
    };
    const QStringList values = {
        QStringLiteral("€49.17"),
        QStringLiteral("€9.83"),
        QStringLiteral("€59.00")
    };

    for (int row = 0; row < labels.size(); ++row) {
        auto* key = makeLabel(labels.at(row), invoiceCard, 0.0, row == 2);
        auto* value = makeLabel(values.at(row), invoiceCard, row == 2 ? 2.0 : 0.0, true);
        key->setObjectName(QStringLiteral("invoiceSummaryKey"));
        totals->addWidget(key, row, 1, Qt::AlignRight);
        totals->addWidget(value, row, 2, Qt::AlignRight);
    }
    invoiceCardLayout->addLayout(totals);

    m_ui->invoiceLayout->addWidget(invoiceCard);

    auto* note = makeCard(QStringLiteral("Notes"), this, 140);
    auto* noteLayout = static_cast<QVBoxLayout*>(note->layout());
    noteLayout->addWidget(makeLabel(
        QStringLiteral("Thank you for your business. Payment is due within 14 days. "
                       "Please include the invoice reference with your transfer."),
        note,
        0.0,
        false));
    m_ui->invoiceLayout->addWidget(note);

    connect(download, &QAbstractButton::clicked, this, [this]() {
        emit messageRequested(QStringLiteral("Invoice PDF generated."));
    });
    connect(pay, &QAbstractButton::clicked, this, [this, status, pay]() {
        status->setText(QStringLiteral("Paid"));
        pay->setEnabled(false);
        emit messageRequested(QStringLiteral("Invoice marked as paid."));
    });
    connect(reminder, &QAbstractButton::clicked, this, [this]() {
        emit messageRequested(QStringLiteral("Payment reminder sent."));
    });

    connect(
        &QtMaterial::ThemeManager::instance(),
        &QtMaterial::ThemeManager::themeChanged,
        this,
        [this](const QtMaterial::Theme&) { applyTheme(); });

    applyTheme();
}

DashboardInvoicePage::~DashboardInvoicePage()
{
    delete m_ui;
}

void DashboardInvoicePage::applyTheme()
{
    const QColor surface = color(QtMaterial::ColorRole::Surface);
    const QColor onSurface = color(QtMaterial::ColorRole::OnSurface);
    const QColor onSurfaceVariant = color(QtMaterial::ColorRole::OnSurfaceVariant);

    setStyleSheet(QStringLiteral(
        "#dashboardContent { background:%1; color:%2; }"
        "#dashboardContent QLabel { color:%2; }"
        "#dashboardContent QLabel#pageSubtitle,"
        "#dashboardContent QLabel#invoiceSummaryKey { color:%3; }")
        .arg(cssColor(surface))
        .arg(cssColor(onSurface))
        .arg(cssColor(onSurfaceVariant)));
}
