#include "dashboardappspage.h"

#include "ui_dashboardappspage.h"

#include <QAbstractButton>
#include <QColor>
#include <QFont>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QShowEvent>
#include <QVector>
#include <QVBoxLayout>

#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledtonalbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h"
#include "qtmaterial/widgets/inputs/qtmaterialsearchbar.h"
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

QtMaterial::QtMaterialCard* makeAppCard(
    const QString& initials,
    const QString& name,
    const QString& category,
    const QString& rating,
    const QString& installs,
    const QString& price,
    QWidget* parent)
{
    auto* card = new QtMaterial::QtMaterialCard(parent);
    card->setVariant(QtMaterial::QtMaterialCard::Variant::Outlined);
    card->setMinimumHeight(230);
    card->setProperty("dashboardAppCategory", category);
    card->setProperty("dashboardAppName", name.toLower());

    auto* layout = new QVBoxLayout(card);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(10);

    auto* top = new QHBoxLayout;
    auto* icon = new QLabel(initials, card);
    icon->setObjectName(QStringLiteral("marketplaceAppIcon"));
    icon->setAlignment(Qt::AlignCenter);
    icon->setFixedSize(54, 54);
    top->addWidget(icon);

    auto* titleBox = new QVBoxLayout;
    titleBox->setSpacing(2);
    titleBox->addWidget(makeLabel(name, card, 2.0, true));
    titleBox->addWidget(makeLabel(category, card, -1.0, false));
    top->addLayout(titleBox, 1);

    auto* priceChip = new QtMaterial::QtMaterialChip(price, card);
    priceChip->setVariant(QtMaterial::ChipVariant::Assist);
    top->addWidget(priceChip, 0, Qt::AlignTop);
    layout->addLayout(top);

    layout->addWidget(makeLabel(
        QStringLiteral("★ %1   •   %2 installs").arg(rating, installs),
        card,
        -1.0,
        false));

    auto* description = makeLabel(
        QStringLiteral("Production-ready integration for your Material desktop workflow."),
        card,
        -1.0,
        false);
    description->setObjectName(QStringLiteral("marketplaceMuted"));
    layout->addWidget(description);
    layout->addStretch(1);

    auto* action = new QtMaterial::QtMaterialFilledTonalButton(
        QStringLiteral("Install"),
        card);
    action->setMinimumWidth(112);
    layout->addWidget(action, 0, Qt::AlignLeft);

    return card;
}

} // namespace

DashboardAppsPage::DashboardAppsPage(QWidget* parent)
    : QWidget(parent)
    , m_ui(new Ui::DashboardAppsPage)
{
    m_ui->setupUi(this);
    setObjectName(QStringLiteral("dashboardContent"));

    auto* heading = new QVBoxLayout;
    heading->setSpacing(2);

    auto* title = makeLabel(QStringLiteral("Apps Marketplace"), this, 5.0, true);
    heading->addWidget(title);

    auto* subtitle = makeLabel(
        QStringLiteral("Discover integrations and desktop tools for your workspace."),
        this,
        -1.0,
        false);
    subtitle->setObjectName(QStringLiteral("pageSubtitle"));
    heading->addWidget(subtitle);

    m_ui->headerLayout->addLayout(heading, 1);

    auto* search = new QtMaterial::QtMaterialSearchBar(this);
    search->setPlaceholderText(QStringLiteral("Search apps..."));
    search->setMinimumWidth(280);
    m_ui->headerLayout->addWidget(search, 0, Qt::AlignTop);

    auto* categories = new QtMaterial::QtMaterialSegmentedButton(this);
    categories->addSegment(QStringLiteral("All"));
    categories->addSegment(QStringLiteral("Productivity"));
    categories->addSegment(QStringLiteral("Design"));
    categories->addSegment(QStringLiteral("Development"));
    categories->setCurrentIndex(0);
    categories->setMinimumWidth(categories->sizeHint().width());
    m_ui->filtersLayout->addWidget(categories);
    m_ui->filtersLayout->addStretch(1);

    const struct {
        const char* initials;
        const char* name;
        const char* category;
        const char* rating;
        const char* installs;
        const char* price;
    } apps[] = {
        {"M3", "Material Studio", "Design", "4.9", "12.4k", "Free"},
        {"GH", "GitHub Connect", "Development", "4.8", "18.9k", "Free"},
        {"NT", "Notion Sync", "Productivity", "4.7", "8.6k", "€12"},
        {"FG", "Figma Bridge", "Design", "4.9", "14.1k", "€19"},
        {"SL", "Slack Workspace", "Productivity", "4.6", "22.7k", "Free"},
        {"CI", "CI Monitor", "Development", "4.8", "6.3k", "€9"}
    };

    QVector<QtMaterial::QtMaterialCard*> cards;
    for (int i = 0; i < 6; ++i) {
        auto* card = makeAppCard(
            QString::fromLatin1(apps[i].initials),
            QString::fromLatin1(apps[i].name),
            QString::fromLatin1(apps[i].category),
            QString::fromLatin1(apps[i].rating),
            QString::fromLatin1(apps[i].installs),
            QString::fromUtf8(apps[i].price),
            this);
        cards.append(card);
        m_ui->appsGrid->addWidget(card, i / 3, i % 3);
        m_ui->appsGrid->setColumnStretch(i % 3, 1);

        const QString appName = QString::fromLatin1(apps[i].name);
        const auto buttons = card->findChildren<QAbstractButton*>();
        for (QAbstractButton* button : buttons) {
            if (button->text() == QStringLiteral("Install")) {
                connect(button, &QAbstractButton::clicked, this, [this, appName, button]() {
                    button->setText(QStringLiteral("Installed"));
                    button->setEnabled(false);
                    emit messageRequested(
                        QStringLiteral("%1 installed.").arg(appName));
                });
            }
        }
    }

    auto* featured = new QtMaterial::QtMaterialCard(this);
    featured->setVariant(QtMaterial::QtMaterialCard::Variant::Filled);
    featured->setMinimumHeight(160);
    auto* featuredLayout = new QHBoxLayout(featured);
    featuredLayout->setContentsMargins(22, 20, 22, 20);
    featuredLayout->setSpacing(18);

    auto* featuredCopy = new QVBoxLayout;
    featuredCopy->setSpacing(6);
    featuredCopy->addWidget(makeLabel(
        QStringLiteral("Featured integration"),
        featured,
        -1.0,
        false));
    featuredCopy->addWidget(makeLabel(
        QStringLiteral("Qt Material Theme Studio"),
        featured,
        4.0,
        true));
    featuredCopy->addWidget(makeLabel(
        QStringLiteral("Build, preview and export polished Material 3 themes directly from your desktop workflow."),
        featured,
        0.0,
        false));
    featuredLayout->addLayout(featuredCopy, 1);

    auto* openFeatured = new QtMaterial::QtMaterialOutlinedButton(
        QStringLiteral("Open studio"),
        featured);
    featuredLayout->addWidget(openFeatured, 0, Qt::AlignVCenter);
    m_ui->featuredLayout->addWidget(featured);

    const auto refreshCards = [cards, search, categories]() {
        const QString needle = search->text().trimmed().toLower();
        const int categoryIndex = categories->currentIndex();

        QString category;
        if (categoryIndex == 1) {
            category = QStringLiteral("Productivity");
        } else if (categoryIndex == 2) {
            category = QStringLiteral("Design");
        } else if (categoryIndex == 3) {
            category = QStringLiteral("Development");
        }

        for (QtMaterial::QtMaterialCard* card : cards) {
            const QString cardCategory =
                card->property("dashboardAppCategory").toString();
            const QString cardName =
                card->property("dashboardAppName").toString();

            const bool categoryMatch =
                category.isEmpty() || cardCategory == category;
            const bool searchMatch =
                needle.isEmpty() || cardName.contains(needle);
            card->setVisible(categoryMatch && searchMatch);
        }
    };

    connect(
        search,
        &QtMaterial::QtMaterialSearchBar::textChanged,
        this,
        [refreshCards](const QString&) { refreshCards(); });
    connect(
        categories,
        &QtMaterial::QtMaterialSegmentedButton::currentIndexChanged,
        this,
        [refreshCards](int) { refreshCards(); });
    connect(openFeatured, &QAbstractButton::clicked, this, [this]() {
        emit messageRequested(QStringLiteral("Theme Studio opened."));
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

void DashboardAppsPage::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    applyTheme();
}

DashboardAppsPage::~DashboardAppsPage()
{
    delete m_ui;
}

void DashboardAppsPage::applyTheme()
{
    const QColor surface = color(QtMaterial::ColorRole::Surface);
    const QColor onSurface = color(QtMaterial::ColorRole::OnSurface);
    const QColor onSurfaceVariant = color(QtMaterial::ColorRole::OnSurfaceVariant);
    const QColor primaryContainer = color(QtMaterial::ColorRole::PrimaryContainer);
    const QColor onPrimaryContainer = color(QtMaterial::ColorRole::OnPrimaryContainer);

    setStyleSheet(QStringLiteral(
        "#dashboardContent { background:%1; color:%2; }"
        "#dashboardContent QLabel { color:%2; }"
        "#dashboardContent QLabel#pageSubtitle,"
        "#dashboardContent QLabel#marketplaceMuted { color:%3; }"
        "#dashboardContent #marketplaceAppIcon { background:%4; color:%5;"
        " border-radius:14px; font-weight:700; font-size:16px; }")
        .arg(cssColor(surface))
        .arg(cssColor(onSurface))
        .arg(cssColor(onSurfaceVariant))
        .arg(cssColor(primaryContainer))
        .arg(cssColor(onPrimaryContainer)));
}
