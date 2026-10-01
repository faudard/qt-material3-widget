#include "dashboardaccountpanel.h"

#include "ui_dashboardaccountpanel.h"

#include <QAbstractButton>
#include <QColor>
#include <QFont>
#include <QFrame>
#include <QLabel>
#include <QPalette>
#include <QShowEvent>
#include <QSizePolicy>
#include <QToolButton>

#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledtonalbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"

namespace {

QString cssColor(const QColor& color)
{
    return color.name(QColor::HexRgb);
}

QToolButton* accountChip(const QString& text, QWidget* parent)
{
    auto* chip = new QToolButton(parent);
    chip->setText(text);
    chip->setFixedSize(42, 42);
    chip->setCursor(Qt::PointingHandCursor);
    chip->setObjectName(QStringLiteral("accountSwitcher"));
    return chip;
}

} // namespace

DashboardAccountPanel::DashboardAccountPanel(QWidget* parent)
    : QWidget(parent)
    , m_ui(new Ui::DashboardAccountPanel)
{
    m_ui->setupUi(this);
    setObjectName(QStringLiteral("dashboardAccountPanel"));
    setAttribute(Qt::WA_StyledBackground, true);
    setAutoFillBackground(true);

    QFont nameFont = m_ui->nameLabel->font();
    nameFont.setBold(true);
    nameFont.setPointSizeF(nameFont.pointSizeF() + 2.0);
    m_ui->nameLabel->setFont(nameFont);

    QFont promoFont = m_ui->promoTitle->font();
    promoFont.setBold(true);
    promoFont.setPointSizeF(promoFont.pointSizeF() + 4.0);
    m_ui->promoTitle->setFont(promoFont);

    auto* close = new QToolButton(this);
    close->setText(QStringLiteral("×"));
    close->setToolTip(QStringLiteral("Close account panel"));
    close->setAccessibleName(QStringLiteral("Close account panel"));
    close->setCursor(Qt::PointingHandCursor);
    close->setFixedSize(36, 36);
    close->setObjectName(QStringLiteral("accountCloseButton"));
    m_ui->headerLayout->addWidget(close);

    const QStringList accountNames = {
        QStringLiteral("JD"),
        QStringLiteral("AM"),
        QStringLiteral("ER")
    };
    for (const QString& name : accountNames) {
        auto* chip = accountChip(name, this);
        m_ui->accountsLayout->insertWidget(
            m_ui->accountsLayout->count() - 1,
            chip);
        connect(chip, &QToolButton::clicked, this, [this, name]() {
            emit messageRequested(
                QStringLiteral("Switched preview account to %1.").arg(name));
        });
    }
    auto* addAccount = accountChip(QStringLiteral("+"), this);
    m_ui->accountsLayout->insertWidget(
        m_ui->accountsLayout->count() - 1,
        addAccount);
    connect(addAccount, &QToolButton::clicked, this, [this]() {
        emit messageRequested(QStringLiteral("Add account action triggered."));
    });

    struct Destination {
        const char* label;
        int page;
    };
    const Destination destinations[] = {
        {"Home", 0},
        {"Profile", 5},
        {"Projects", 13},
        {"Subscription", 6},
        {"Security", 9},
        {"Account settings", 9}
    };

    for (const auto& destination : destinations) {
        auto* button = new QToolButton(this);
        button->setText(QString::fromLatin1(destination.label));
        button->setMinimumHeight(42);
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        button->setToolButtonStyle(Qt::ToolButtonTextOnly);
        button->setCursor(Qt::PointingHandCursor);
        button->setObjectName(QStringLiteral("accountMenuButton"));
        m_ui->menuLayout->addWidget(button);
        const int page = destination.page;
        connect(button, &QAbstractButton::clicked, this, [this, page]() {
            emit navigateRequested(page);
        });
    }

    auto* upgrade = new QtMaterial::QtMaterialFilledTonalButton(
        QStringLiteral("Upgrade to Pro"),
        m_ui->promoFrame);
    m_ui->promoLayout->addWidget(upgrade, 0, Qt::AlignLeft);
    connect(upgrade, &QAbstractButton::clicked, this, [this]() {
        emit navigateRequested(6);
    });

    auto* logout = new QtMaterial::QtMaterialOutlinedButton(
        QStringLiteral("Logout"),
        this);
    logout->setMinimumHeight(46);
    m_ui->logoutLayout->addWidget(logout);
    connect(logout, &QAbstractButton::clicked, this, [this]() {
        emit messageRequested(QStringLiteral("Logout action triggered."));
    });

    connect(close, &QToolButton::clicked, this, &DashboardAccountPanel::closeRequested);

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

void DashboardAccountPanel::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    applyTheme();
}

DashboardAccountPanel::~DashboardAccountPanel()
{
    delete m_ui;
}

void DashboardAccountPanel::applyTheme()
{
    const auto& scheme =
        QtMaterial::ThemeManager::instance().theme().colorScheme();

    const QColor surface =
        scheme.color(QtMaterial::ColorRole::Surface);
    const QColor onSurface =
        scheme.color(QtMaterial::ColorRole::OnSurface);
    const QColor onSurfaceVariant =
        scheme.color(QtMaterial::ColorRole::OnSurfaceVariant);
    const QColor primary =
        scheme.color(QtMaterial::ColorRole::Primary);
    const QColor primaryContainer =
        scheme.color(QtMaterial::ColorRole::PrimaryContainer);
    const QColor onPrimaryContainer =
        scheme.color(QtMaterial::ColorRole::OnPrimaryContainer);
    const QColor tertiaryContainer =
        scheme.color(QtMaterial::ColorRole::TertiaryContainer);
    const QColor onTertiaryContainer =
        scheme.color(QtMaterial::ColorRole::OnTertiaryContainer);
    const QColor outline =
        scheme.color(QtMaterial::ColorRole::OutlineVariant);

    setStyleSheet(QStringLiteral(
        "#dashboardAccountPanel { background:%1; border-left:1px solid %7; }"
        "#dashboardAccountPanel QLabel { color:%2; }"
        "#dashboardAccountPanel #emailLabel { color:%3; }"
        "#dashboardAccountPanel #avatarLabel { background:%4; color:%5;"
        " border:2px solid %6; border-radius:39px; font-size:20px; font-weight:700; }"
        "#dashboardAccountPanel #accountCloseButton { background:transparent; color:%3;"
        " border:0; border-radius:18px; font-size:25px; }"
        "#dashboardAccountPanel #accountCloseButton:hover { background:%4; color:%5; }"
        "#dashboardAccountPanel #accountSwitcher { background:%4; color:%5;"
        " border:1px solid %7; border-radius:21px; font-weight:600; }"
        "#dashboardAccountPanel #accountSwitcher:hover { border-color:%6; }"
        "#dashboardAccountPanel #accountMenuButton { background:transparent; color:%2;"
        " border:0; border-radius:10px; padding:9px 12px; text-align:left; font-weight:500; }"
        "#dashboardAccountPanel #accountMenuButton:hover { background:%4; color:%5; }"
        "#dashboardAccountPanel #promoFrame {"
        " background:qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 %8,stop:1 %4);"
        " border:0; border-radius:14px; }"
        "#dashboardAccountPanel #promoFrame QLabel { color:%9; }"
        "#dashboardAccountPanel #topSeparator { color:%7; }")
        .arg(cssColor(surface))
        .arg(cssColor(onSurface))
        .arg(cssColor(onSurfaceVariant))
        .arg(cssColor(primaryContainer))
        .arg(cssColor(onPrimaryContainer))
        .arg(cssColor(primary))
        .arg(cssColor(outline))
        .arg(cssColor(tertiaryContainer))
        .arg(cssColor(onTertiaryContainer)));

    QPalette panelPalette = this->palette();
    panelPalette.setColor(QPalette::Window, surface);
    panelPalette.setColor(QPalette::WindowText, onSurface);
    panelPalette.setColor(QPalette::Text, onSurface);
    setPalette(panelPalette);
    setAutoFillBackground(true);
}
