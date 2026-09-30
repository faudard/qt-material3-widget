#include "dashboardaccountpage.h"

#include "ui_dashboardaccountpage.h"

#include <QAbstractButton>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

#include "qtmaterial/widgets/buttons/qtmaterialfilledtonalbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h"
#include "qtmaterial/widgets/selection/qtmaterialchip.h"
#include "qtmaterial/widgets/selection/qtmaterialswitch.h"
#include "qtmaterial/widgets/surfaces/qtmaterialcard.h"

namespace {

QLabel* label(
    const QString& text,
    QWidget* parent,
    qreal pointDelta = 0.0,
    bool bold = false)
{
    auto* result = new QLabel(text, parent);
    QFont font = result->font();
    font.setPointSizeF(qMax<qreal>(8.0, font.pointSizeF() + pointDelta));
    font.setBold(bold);
    result->setFont(font);
    result->setWordWrap(true);
    return result;
}

QtMaterial::QtMaterialCard* makeCard(
    const QString& title,
    QWidget* parent)
{
    auto* card = new QtMaterial::QtMaterialCard(parent);
    card->setVariant(QtMaterial::QtMaterialCard::Variant::Outlined);
    card->setMinimumHeight(210);
    auto* layout = new QVBoxLayout(card);
    layout->setContentsMargins(20, 18, 20, 20);
    layout->setSpacing(9);
    layout->addWidget(label(title, card, 2.0, true));
    return card;
}

} // namespace

DashboardAccountPage::DashboardAccountPage(QWidget* parent)
    : QWidget(parent)
    , m_ui(new Ui::DashboardAccountPage)
{
    m_ui->setupUi(this);
    setObjectName(QStringLiteral("dashboardContent"));

    QFont titleFont = m_ui->titleLabel->font();
    titleFont.setPointSizeF(titleFont.pointSizeF() + 5.0);
    titleFont.setBold(true);
    m_ui->titleLabel->setFont(titleFont);
    m_ui->subtitleLabel->setObjectName(QStringLiteral("pageSubtitle"));

    auto* identity = makeCard(QStringLiteral("Account overview"), this);
    auto* identityLayout = qobject_cast<QVBoxLayout*>(identity->layout());
    identityLayout->addWidget(label(QStringLiteral("John Doe"), identity, 4.0, true));
    identityLayout->addWidget(label(QStringLiteral("john.doe@example.com"), identity, -1.0, false));
    auto* plan = new QtMaterial::QtMaterialChip(QStringLiteral("Professional plan"), identity);
    plan->setVariant(QtMaterial::ChipVariant::Assist);
    identityLayout->addWidget(plan, 0, Qt::AlignLeft);
    identityLayout->addStretch(1);

    auto* security = makeCard(QStringLiteral("Security"), this);
    auto* securityLayout = qobject_cast<QVBoxLayout*>(security->layout());
    auto* twoFactor = new QtMaterial::QtMaterialSwitch(
        QStringLiteral("Two-factor authentication"),
        security);
    twoFactor->setChecked(true);
    securityLayout->addWidget(twoFactor);
    securityLayout->addWidget(label(
        QStringLiteral("Last sign-in: today at 17:42 from Windows desktop"),
        security,
        -1.0,
        false));
    auto* password = new QtMaterial::QtMaterialOutlinedButton(
        QStringLiteral("Change password"),
        security);
    securityLayout->addWidget(password, 0, Qt::AlignLeft);
    securityLayout->addStretch(1);

    auto* billing = makeCard(QStringLiteral("Billing"), this);
    auto* billingLayout = qobject_cast<QVBoxLayout*>(billing->layout());
    billingLayout->addWidget(label(QStringLiteral("Professional · €29 / month"), billing, 1.0, true));
    billingLayout->addWidget(label(QStringLiteral("Next invoice: 15 October 2026"), billing, -1.0, false));
    billingLayout->addWidget(label(QStringLiteral("Payment method: Visa •••• 2048"), billing, -1.0, false));
    auto* manageBilling = new QtMaterial::QtMaterialFilledTonalButton(
        QStringLiteral("Manage billing"),
        billing);
    billingLayout->addWidget(manageBilling, 0, Qt::AlignLeft);
    billingLayout->addStretch(1);

    auto* sessions = makeCard(QStringLiteral("Active sessions"), this);
    auto* sessionsLayout = qobject_cast<QVBoxLayout*>(sessions->layout());
    sessionsLayout->addWidget(label(QStringLiteral("Windows 10 · current session"), sessions, 0.0, true));
    sessionsLayout->addWidget(label(QStringLiteral("Toulouse area · active now"), sessions, -1.0, false));
    sessionsLayout->addSpacing(8);
    sessionsLayout->addWidget(label(QStringLiteral("Ubuntu desktop"), sessions, 0.0, true));
    sessionsLayout->addWidget(label(QStringLiteral("Last active 2 hours ago"), sessions, -1.0, false));
    sessionsLayout->addStretch(1);

    m_ui->cardsLayout->addWidget(identity, 0, 0);
    m_ui->cardsLayout->addWidget(security, 0, 1);
    m_ui->cardsLayout->addWidget(billing, 1, 0);
    m_ui->cardsLayout->addWidget(sessions, 1, 1);
    m_ui->cardsLayout->setColumnStretch(0, 1);
    m_ui->cardsLayout->setColumnStretch(1, 1);

    auto* editProfile = new QtMaterial::QtMaterialFilledTonalButton(
        QStringLiteral("Edit profile"),
        this);
    m_ui->actionsLayout->addWidget(editProfile);

    connect(editProfile, &QAbstractButton::clicked, this, &DashboardAccountPage::editProfileRequested);
    connect(password, &QAbstractButton::clicked, this, [this]() {
        emit messageRequested(QStringLiteral("Password change flow opened."));
    });
    connect(manageBilling, &QAbstractButton::clicked, this, [this]() {
        emit messageRequested(QStringLiteral("Billing management opened."));
    });
    connect(twoFactor, &QAbstractButton::toggled, this, [this](bool checked) {
        emit messageRequested(
            checked
                ? QStringLiteral("Two-factor authentication enabled.")
                : QStringLiteral("Two-factor authentication disabled."));
    });
}

DashboardAccountPage::~DashboardAccountPage()
{
    delete m_ui;
}
