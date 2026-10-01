#include "dashboardnotificationspanel.h"

#include "ui_dashboardnotificationspanel.h"

#include <QAbstractButton>
#include <QColor>
#include <QFont>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPalette>
#include <QToolButton>
#include <QVBoxLayout>

#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"
#include "qtmaterial/widgets/selection/qtmaterialsegmentedbutton.h"

namespace {

QString cssColor(const QColor& color)
{
    return color.name(QColor::HexRgb);
}

QWidget* notificationCard(
    const QString& initials,
    const QString& title,
    const QString& meta,
    bool unread,
    QWidget* parent)
{
    auto* card = new QFrame(parent);
    card->setObjectName(QStringLiteral("notificationCard"));
    card->setProperty("notificationUnread", unread);

    auto* layout = new QHBoxLayout(card);
    layout->setContentsMargins(10, 12, 10, 12);
    layout->setSpacing(12);

    auto* avatar = new QLabel(initials, card);
    avatar->setObjectName(QStringLiteral("notificationAvatar"));
    avatar->setAlignment(Qt::AlignCenter);
    avatar->setFixedSize(48, 48);
    layout->addWidget(avatar, 0, Qt::AlignTop);

    auto* copy = new QVBoxLayout;
    copy->setSpacing(5);

    auto* titleLabel = new QLabel(title, card);
    titleLabel->setWordWrap(true);
    QFont titleFont = titleLabel->font();
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    copy->addWidget(titleLabel);

    auto* metaLabel = new QLabel(meta, card);
    metaLabel->setObjectName(QStringLiteral("notificationMeta"));
    copy->addWidget(metaLabel);

    layout->addLayout(copy, 1);

    if (unread) {
        auto* dot = new QLabel(QStringLiteral("●"), card);
        dot->setObjectName(QStringLiteral("notificationUnreadDot"));
        layout->addWidget(dot, 0, Qt::AlignTop);
    }

    return card;
}

} // namespace

DashboardNotificationsPanel::DashboardNotificationsPanel(QWidget* parent)
    : QWidget(parent)
    , m_ui(new Ui::DashboardNotificationsPanel)
{
    m_ui->setupUi(this);
    setObjectName(QStringLiteral("dashboardNotificationsPanel"));

    QFont titleFont = m_ui->titleLabel->font();
    titleFont.setBold(true);
    titleFont.setPointSizeF(titleFont.pointSizeF() + 4.0);
    m_ui->titleLabel->setFont(titleFont);

    auto* markAll = new QToolButton(this);
    markAll->setText(QStringLiteral("✓✓"));
    markAll->setToolTip(QStringLiteral("Mark all as read"));
    markAll->setAccessibleName(QStringLiteral("Mark all notifications as read"));
    markAll->setObjectName(QStringLiteral("panelActionButton"));
    markAll->setFixedSize(36, 36);

    auto* settings = new QToolButton(this);
    settings->setText(QStringLiteral("⚙"));
    settings->setToolTip(QStringLiteral("Notification settings"));
    settings->setAccessibleName(QStringLiteral("Notification settings"));
    settings->setObjectName(QStringLiteral("panelActionButton"));
    settings->setFixedSize(36, 36);

    auto* close = new QToolButton(this);
    close->setText(QStringLiteral("×"));
    close->setToolTip(QStringLiteral("Close notifications"));
    close->setAccessibleName(QStringLiteral("Close notifications"));
    close->setObjectName(QStringLiteral("panelCloseButton"));
    close->setFixedSize(36, 36);

    m_ui->headerLayout->addWidget(markAll);
    m_ui->headerLayout->addWidget(settings);
    m_ui->headerLayout->addWidget(close);

    auto* tabs = new QtMaterial::QtMaterialSegmentedButton(this);
    tabs->addSegment(QStringLiteral("All  22"));
    tabs->addSegment(QStringLiteral("Unread  12"));
    tabs->addSegment(QStringLiteral("Archived  10"));
    tabs->setCurrentIndex(0);
    tabs->setMinimumWidth(tabs->sizeHint().width());
    m_ui->tabsLayout->addWidget(tabs, 1);

    auto* friendRequest = notificationCard(
        QStringLiteral("DB"),
        QStringLiteral("Deja Brady sent you a friend request"),
        QStringLiteral("a few seconds · Communication"),
        true,
        m_ui->scrollContent);
    auto* friendActions = new QHBoxLayout;
    auto* accept = new QtMaterial::QtMaterialFilledButton(
        QStringLiteral("Accept"),
        friendRequest);
    auto* decline = new QtMaterial::QtMaterialOutlinedButton(
        QStringLiteral("Decline"),
        friendRequest);
    friendActions->addWidget(accept);
    friendActions->addWidget(decline);
    friendActions->addStretch(1);
    if (auto* copy = static_cast<QVBoxLayout*>(friendRequest->layout()->itemAt(1)->layout())) {
        copy->addLayout(friendActions);
    }
    m_ui->notificationsLayout->addWidget(friendRequest);

    auto* mention = notificationCard(
        QStringLiteral("JH"),
        QStringLiteral("Jayvon Hull mentioned you in Minimal UI"),
        QStringLiteral("a day · Project UI"),
        true,
        m_ui->scrollContent);
    if (auto* copy = static_cast<QVBoxLayout*>(mention->layout()->itemAt(1)->layout())) {
        auto* quote = new QLabel(
            QStringLiteral("@John Doe — feedback by asking questions or just leave a note of appreciation."),
            mention);
        quote->setObjectName(QStringLiteral("notificationQuote"));
        quote->setWordWrap(true);
        copy->addWidget(quote);
        auto* reply = new QtMaterial::QtMaterialFilledButton(
            QStringLiteral("Reply"),
            mention);
        copy->addWidget(reply, 0, Qt::AlignLeft);
        connect(reply, &QAbstractButton::clicked, this, [this]() {
            emit messageRequested(QStringLiteral("Reply composer opened."));
        });
    }
    m_ui->notificationsLayout->addWidget(mention);

    auto* fileAdded = notificationCard(
        QStringLiteral("LD"),
        QStringLiteral("Lainey Davidson added a file to File manager"),
        QStringLiteral("2 days · File manager"),
        true,
        m_ui->scrollContent);
    if (auto* copy = static_cast<QVBoxLayout*>(fileAdded->layout()->itemAt(1)->layout())) {
        auto* download = new QtMaterial::QtMaterialOutlinedButton(
            QStringLiteral("Download"),
            fileAdded);
        copy->addWidget(download, 0, Qt::AlignLeft);
        connect(download, &QAbstractButton::clicked, this, [this]() {
            emit messageRequested(QStringLiteral("Download started."));
        });
    }
    m_ui->notificationsLayout->addWidget(fileAdded);

    auto* archived = notificationCard(
        QStringLiteral("AK"),
        QStringLiteral("Ava Kim completed the Q4 review"),
        QStringLiteral("4 days · Team"),
        false,
        m_ui->scrollContent);
    m_ui->notificationsLayout->addWidget(archived);
    m_ui->notificationsLayout->addStretch(1);

    auto* viewAll = new QtMaterial::QtMaterialTextButton(
        QStringLiteral("View all"),
        this);
    m_ui->footerLayout->addWidget(viewAll);

    const auto refreshFilter = [this, tabs]() {
        const int index = tabs->currentIndex();
        const auto cards = findChildren<QFrame*>(QStringLiteral("notificationCard"));
        for (QFrame* card : cards) {
            const bool unread = card->property("notificationUnread").toBool();
            const bool visible =
                index == 0
                || (index == 1 && unread)
                || (index == 2 && !unread);
            card->setVisible(visible);
        }
    };

    connect(tabs, &QtMaterial::QtMaterialSegmentedButton::currentIndexChanged, this, [refreshFilter](int) {
        refreshFilter();
    });
    connect(close, &QToolButton::clicked, this, &DashboardNotificationsPanel::closeRequested);
    connect(settings, &QToolButton::clicked, this, &DashboardNotificationsPanel::settingsRequested);
    connect(markAll, &QToolButton::clicked, this, [this]() {
        const auto dots = findChildren<QLabel*>(QStringLiteral("notificationUnreadDot"));
        for (QLabel* dot : dots) {
            dot->hide();
        }
        emit messageRequested(QStringLiteral("All notifications marked as read."));
    });
    connect(accept, &QAbstractButton::clicked, this, [this]() {
        emit messageRequested(QStringLiteral("Friend request accepted."));
    });
    connect(decline, &QAbstractButton::clicked, this, [this]() {
        emit messageRequested(QStringLiteral("Friend request declined."));
    });
    connect(viewAll, &QAbstractButton::clicked, this, [this]() {
        emit messageRequested(QStringLiteral("Notifications center opened."));
    });

    connect(
        &QtMaterial::ThemeManager::instance(),
        &QtMaterial::ThemeManager::themeChanged,
        this,
        [this](const QtMaterial::Theme&) { applyTheme(); });

    applyTheme();
}

DashboardNotificationsPanel::~DashboardNotificationsPanel()
{
    delete m_ui;
}

void DashboardNotificationsPanel::applyTheme()
{
    const auto& scheme = QtMaterial::ThemeManager::instance().theme().colorScheme();
    const QColor surface = scheme.color(QtMaterial::ColorRole::Surface);
    const QColor surfaceContainer = scheme.color(QtMaterial::ColorRole::SurfaceContainerLow);
    const QColor onSurface = scheme.color(QtMaterial::ColorRole::OnSurface);
    const QColor onSurfaceVariant = scheme.color(QtMaterial::ColorRole::OnSurfaceVariant);
    const QColor primary = scheme.color(QtMaterial::ColorRole::Primary);
    const QColor primaryContainer = scheme.color(QtMaterial::ColorRole::PrimaryContainer);
    const QColor onPrimaryContainer = scheme.color(QtMaterial::ColorRole::OnPrimaryContainer);
    const QColor outline = scheme.color(QtMaterial::ColorRole::OutlineVariant);

    setStyleSheet(QStringLiteral(
        "#dashboardNotificationsPanel { background:%1; border-left:1px solid %5; }"
        "#dashboardNotificationsPanel QLabel { color:%2; }"
        "#dashboardNotificationsPanel #notificationMeta { color:%3; }"
        "#dashboardNotificationsPanel #notificationCard { background:%4; border:1px solid %5; border-radius:12px; }"
        "#dashboardNotificationsPanel #notificationAvatar { background:%6; color:%7; border-radius:24px; font-weight:700; }"
        "#dashboardNotificationsPanel #notificationUnreadDot { color:%8; font-size:16px; }"
        "#dashboardNotificationsPanel #notificationQuote { background:%1; color:%3; border-radius:10px; padding:10px; }"
        "#dashboardNotificationsPanel #panelActionButton,"
        "#dashboardNotificationsPanel #panelCloseButton { background:transparent; color:%3; border:0; border-radius:18px; font-size:20px; }"
        "#dashboardNotificationsPanel #panelActionButton:hover,"
        "#dashboardNotificationsPanel #panelCloseButton:hover { background:%4; }"
        "QScrollArea { background:%1; border:0; }"
        "QScrollArea > QWidget > QWidget { background:%1; }")
        .arg(cssColor(surface))
        .arg(cssColor(onSurface))
        .arg(cssColor(onSurfaceVariant))
        .arg(cssColor(surfaceContainer))
        .arg(cssColor(outline))
        .arg(cssColor(primaryContainer))
        .arg(cssColor(onPrimaryContainer))
        .arg(cssColor(primary)));

    QPalette panelPalette = palette();
    panelPalette.setColor(QPalette::Window, surface);
    panelPalette.setColor(QPalette::WindowText, onSurface);
    setPalette(panelPalette);
    setAutoFillBackground(true);
}
