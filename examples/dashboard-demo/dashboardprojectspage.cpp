#include "dashboardprojectspage.h"

#include "ui_dashboardprojectspage.h"

#include <QAbstractButton>
#include <QColor>
#include <QFont>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QVector>
#include <QVBoxLayout>

#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledtonalbutton.h"
#include "qtmaterial/widgets/inputs/qtmaterialsearchbar.h"
#include "qtmaterial/widgets/progress/qtmateriallinearprogressindicator.h"
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

QtMaterial::QtMaterialCard* createProjectCard(
    const QString& name,
    const QString& category,
    const QString& status,
    const QString& due,
    int progress,
    const QStringList& members,
    QWidget* parent)
{
    auto* card = new QtMaterial::QtMaterialCard(parent);
    card->setVariant(QtMaterial::QtMaterialCard::Variant::Outlined);
    card->setMinimumHeight(260);
    card->setProperty("dashboardProjectStatus", status);
    card->setProperty("dashboardProjectName", name.toLower());

    auto* layout = new QVBoxLayout(card);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(10);

    auto* top = new QHBoxLayout;
    auto* copy = new QVBoxLayout;
    copy->setSpacing(3);
    copy->addWidget(makeLabel(name, card, 2.0, true));
    auto* categoryLabel = makeLabel(category, card, -1.0, false);
    categoryLabel->setObjectName(QStringLiteral("projectMuted"));
    copy->addWidget(categoryLabel);
    top->addLayout(copy, 1);

    auto* statusChip = new QtMaterial::QtMaterialChip(status, card);
    statusChip->setVariant(QtMaterial::ChipVariant::Assist);
    top->addWidget(statusChip, 0, Qt::AlignTop);
    layout->addLayout(top);

    auto* progressLabel = makeLabel(
        QStringLiteral("%1% complete").arg(progress),
        card,
        -1.0,
        true);
    layout->addWidget(progressLabel);

    auto* progressBar =
        new QtMaterial::QtMaterialLinearProgressIndicator(card);
    progressBar->setValue(static_cast<qreal>(progress) / 100.0);
    progressBar->setStatusText(
        QStringLiteral("%1 percent complete").arg(progress));
    layout->addWidget(progressBar);

    auto* meta = new QHBoxLayout;
    auto* dueLabel = makeLabel(
        QStringLiteral("Due %1").arg(due),
        card,
        -1.0,
        false);
    dueLabel->setObjectName(QStringLiteral("projectMuted"));
    meta->addWidget(dueLabel);
    meta->addStretch(1);

    for (const QString& member : members) {
        auto* avatar = new QLabel(member, card);
        avatar->setObjectName(QStringLiteral("projectAvatar"));
        avatar->setAlignment(Qt::AlignCenter);
        avatar->setFixedSize(30, 30);
        meta->addWidget(avatar);
    }
    layout->addLayout(meta);

    layout->addStretch(1);

    auto* open = new QtMaterial::QtMaterialFilledTonalButton(
        QStringLiteral("Open project"),
        card);
    layout->addWidget(open, 0, Qt::AlignLeft);

    return card;
}

} // namespace

DashboardProjectsPage::DashboardProjectsPage(QWidget* parent)
    : QWidget(parent)
    , m_ui(new Ui::DashboardProjectsPage)
{
    m_ui->setupUi(this);
    setObjectName(QStringLiteral("dashboardContent"));

    auto* heading = new QVBoxLayout;
    heading->setSpacing(2);
    heading->addWidget(makeLabel(
        QStringLiteral("Projects"),
        this,
        5.0,
        true));

    auto* subtitle = makeLabel(
        QStringLiteral("Track delivery, team activity and project health."),
        this,
        -1.0,
        false);
    subtitle->setObjectName(QStringLiteral("pageSubtitle"));
    heading->addWidget(subtitle);
    m_ui->headerLayout->addLayout(heading, 1);

    auto* search = new QtMaterial::QtMaterialSearchBar(this);
    search->setPlaceholderText(QStringLiteral("Search projects..."));
    search->setMinimumWidth(280);
    m_ui->headerLayout->addWidget(search, 0, Qt::AlignTop);

    auto* create = new QtMaterial::QtMaterialFilledButton(
        QStringLiteral("New project"),
        this);
    m_ui->headerLayout->addWidget(create, 0, Qt::AlignTop);

    auto* filters = new QtMaterial::QtMaterialSegmentedButton(this);
    filters->addSegment(QStringLiteral("All"));
    filters->addSegment(QStringLiteral("Active"));
    filters->addSegment(QStringLiteral("At risk"));
    filters->addSegment(QStringLiteral("Completed"));
    filters->setCurrentIndex(0);
    filters->setMinimumWidth(filters->sizeHint().width());
    m_ui->filtersLayout->addWidget(filters);
    m_ui->filtersLayout->addStretch(1);

    const struct {
        const char* name;
        const char* category;
        const char* status;
        const char* due;
        int progress;
        const char* members[3];
        int memberCount;
    } projects[] = {
        {"Material 3 Desktop", "Design system", "Active", "08 Oct", 72, {"JD","AM","ER"}, 3},
        {"Theme Studio 1.0", "Developer tools", "Active", "16 Oct", 58, {"JD","LS","MK"}, 3},
        {"Accessibility audit", "Quality", "At risk", "04 Oct", 41, {"ER","DB",""}, 2},
        {"Dashboard showcase", "Examples", "Active", "12 Oct", 86, {"JD","AM",""}, 2},
        {"Qt 6 migration", "Platform", "Completed", "28 Sep", 100, {"LS","MK",""}, 2},
        {"Packaging refresh", "Release", "At risk", "06 Oct", 33, {"DB","JD",""}, 2}
    };

    QVector<QtMaterial::QtMaterialCard*> cards;
    for (int i = 0; i < 6; ++i) {
        QStringList members;
        for (int m = 0; m < projects[i].memberCount; ++m) {
            members.append(QString::fromLatin1(projects[i].members[m]));
        }

        auto* card = createProjectCard(
            QString::fromLatin1(projects[i].name),
            QString::fromLatin1(projects[i].category),
            QString::fromLatin1(projects[i].status),
            QString::fromLatin1(projects[i].due),
            projects[i].progress,
            members,
            this);
        cards.append(card);
        m_ui->projectsGrid->addWidget(card, i / 3, i % 3);
        m_ui->projectsGrid->setColumnStretch(i % 3, 1);

        const QString projectName = QString::fromLatin1(projects[i].name);
        const auto buttons = card->findChildren<QAbstractButton*>();
        for (QAbstractButton* button : buttons) {
            if (button->text() == QStringLiteral("Open project")) {
                connect(
                    button,
                    &QAbstractButton::clicked,
                    this,
                    [this, projectName]() {
                        emit messageRequested(
                            QStringLiteral("%1 opened.").arg(projectName));
                    });
            }
        }
    }

    const auto refreshCards = [cards, search, filters]() {
        const QString needle = search->text().trimmed().toLower();
        const int filter = filters->currentIndex();

        QString status;
        if (filter == 1) {
            status = QStringLiteral("Active");
        } else if (filter == 2) {
            status = QStringLiteral("At risk");
        } else if (filter == 3) {
            status = QStringLiteral("Completed");
        }

        for (QtMaterial::QtMaterialCard* card : cards) {
            const QString cardStatus =
                card->property("dashboardProjectStatus").toString();
            const QString cardName =
                card->property("dashboardProjectName").toString();

            const bool statusMatch =
                status.isEmpty() || cardStatus == status;
            const bool searchMatch =
                needle.isEmpty() || cardName.contains(needle);
            card->setVisible(statusMatch && searchMatch);
        }
    };

    connect(
        search,
        &QtMaterial::QtMaterialSearchBar::textChanged,
        this,
        [refreshCards](const QString&) { refreshCards(); });
    connect(
        filters,
        &QtMaterial::QtMaterialSegmentedButton::currentIndexChanged,
        this,
        [refreshCards](int) { refreshCards(); });
    connect(create, &QAbstractButton::clicked, this, [this]() {
        emit messageRequested(QStringLiteral("New project flow opened."));
    });

    auto* summary = new QtMaterial::QtMaterialCard(this);
    summary->setVariant(QtMaterial::QtMaterialCard::Variant::Filled);
    summary->setMinimumHeight(128);
    auto* summaryLayout = new QHBoxLayout(summary);
    summaryLayout->setContentsMargins(20, 18, 20, 18);
    summaryLayout->setSpacing(28);

    const struct {
        const char* label;
        const char* value;
    } summaryData[] = {
        {"Active", "3"},
        {"At risk", "2"},
        {"Completed", "1"},
        {"Avg. progress", "65%"}
    };

    for (const auto& item : summaryData) {
        auto* box = new QVBoxLayout;
        box->setSpacing(3);
        box->addWidget(makeLabel(
            QString::fromLatin1(item.value),
            summary,
            5.0,
            true));
        auto* key = makeLabel(
            QString::fromLatin1(item.label),
            summary,
            -1.0,
            false);
        key->setObjectName(QStringLiteral("projectMuted"));
        box->addWidget(key);
        summaryLayout->addLayout(box);
    }
    summaryLayout->addStretch(1);
    m_ui->footerLayout->addWidget(summary);

    connect(
        &QtMaterial::ThemeManager::instance(),
        &QtMaterial::ThemeManager::themeChanged,
        this,
        [this](const QtMaterial::Theme&) { applyTheme(); });

    applyTheme();
}

DashboardProjectsPage::~DashboardProjectsPage()
{
    delete m_ui;
}

void DashboardProjectsPage::applyTheme()
{
    const QColor surface = color(QtMaterial::ColorRole::Surface);
    const QColor onSurface = color(QtMaterial::ColorRole::OnSurface);
    const QColor onSurfaceVariant =
        color(QtMaterial::ColorRole::OnSurfaceVariant);
    const QColor primaryContainer =
        color(QtMaterial::ColorRole::PrimaryContainer);
    const QColor onPrimaryContainer =
        color(QtMaterial::ColorRole::OnPrimaryContainer);

    setStyleSheet(QStringLiteral(
        "#dashboardContent { background:%1; color:%2; }"
        "#dashboardContent QLabel { color:%2; }"
        "#dashboardContent QLabel#pageSubtitle,"
        "#dashboardContent QLabel#projectMuted { color:%3; }"
        "#dashboardContent QLabel#projectAvatar {"
        " background:%4; color:%5; border-radius:15px;"
        " font-size:10px; font-weight:700; }")
        .arg(cssColor(surface))
        .arg(cssColor(onSurface))
        .arg(cssColor(onSurfaceVariant))
        .arg(cssColor(primaryContainer))
        .arg(cssColor(onPrimaryContainer)));
}
