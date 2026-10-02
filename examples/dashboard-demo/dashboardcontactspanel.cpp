#include "dashboardcontactspanel.h"

#include "ui_dashboardcontactspanel.h"

#include <QAbstractButton>
#include <QColor>
#include <QFont>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPalette>
#include <QShowEvent>
#include <QToolButton>
#include <QVBoxLayout>

#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"

namespace {

QString cssColor(const QColor& color)
{
    return color.name(QColor::HexRgb);
}

QWidget* contactRow(
    const QString& initials,
    const QString& name,
    const QString& activity,
    const QString& status,
    QWidget* parent)
{
    auto* row = new QFrame(parent);
    row->setObjectName(QStringLiteral("contactRow"));
    row->setMinimumHeight(74);

    auto* layout = new QHBoxLayout(row);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(12);

    auto* avatar = new QLabel(initials, row);
    avatar->setObjectName(QStringLiteral("contactAvatar"));
    avatar->setAlignment(Qt::AlignCenter);
    avatar->setFixedSize(48, 48);
    layout->addWidget(avatar);

    auto* text = new QVBoxLayout;
    text->setSpacing(2);

    auto* nameLabel = new QLabel(name, row);
    QFont nameFont = nameLabel->font();
    nameFont.setBold(true);
    nameFont.setPointSizeF(nameFont.pointSizeF() + 1.0);
    nameLabel->setFont(nameFont);
    text->addWidget(nameLabel);

    if (!activity.isEmpty()) {
        auto* activityLabel = new QLabel(activity, row);
        activityLabel->setObjectName(QStringLiteral("contactActivity"));
        text->addWidget(activityLabel);
    }

    layout->addLayout(text, 1);

    auto* dot = new QLabel(QStringLiteral("●"), row);
    dot->setObjectName(QStringLiteral("contactStatus"));
    dot->setProperty("contactStatusKind", status);
    layout->addWidget(dot, 0, Qt::AlignVCenter);

    return row;
}

} // namespace

DashboardContactsPanel::DashboardContactsPanel(QWidget* parent)
    : QWidget(parent)
    , m_ui(new Ui::DashboardContactsPanel)
{
    m_ui->setupUi(this);
    setObjectName(QStringLiteral("dashboardContactsPanel"));
    setAttribute(Qt::WA_StyledBackground, true);
    setAutoFillBackground(true);

    QFont titleFont = m_ui->titleLabel->font();
    titleFont.setBold(true);
    titleFont.setPointSizeF(titleFont.pointSizeF() + 4.0);
    m_ui->titleLabel->setFont(titleFont);

    auto* close = new QToolButton(this);
    close->setText(QStringLiteral("×"));
    close->setToolTip(QStringLiteral("Close contacts"));
    close->setAccessibleName(QStringLiteral("Close contacts"));
    close->setObjectName(QStringLiteral("panelCloseButton"));
    close->setFixedSize(36, 36);
    close->setCursor(Qt::PointingHandCursor);
    m_ui->headerLayout->addWidget(close);

    const struct {
        const char* initials;
        const char* name;
        const char* activity;
        const char* status;
    } contacts[] = {
        {"JS", "Jayvion Simon", "Available", "busy"},
        {"LO", "Lucian O'Brien", "Online", "online"},
        {"DB", "Deja Brady", "2 days", "away"},
        {"HS", "Harrison Stein", "Online", "online"},
        {"RC", "Reece Chung", "4 days", "away"},
        {"AD", "Avery Diaz", "Yesterday", "away"},
        {"MK", "Mia Klein", "Online", "online"}
    };

    for (const auto& contact : contacts) {
        m_ui->contactsLayout->addWidget(contactRow(
            QString::fromLatin1(contact.initials),
            QString::fromLatin1(contact.name),
            QString::fromLatin1(contact.activity),
            QString::fromLatin1(contact.status),
            m_ui->scrollContent));
    }
    m_ui->contactsLayout->addStretch(1);

    auto* viewAll = new QtMaterial::QtMaterialTextButton(
        QStringLiteral("View all contacts"),
        this);
    m_ui->footerLayout->addWidget(viewAll);

    connect(close, &QToolButton::clicked, this, &DashboardContactsPanel::closeRequested);
    connect(viewAll, &QAbstractButton::clicked, this, [this]() {
        emit messageRequested(QStringLiteral("Contacts directory opened."));
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

void DashboardContactsPanel::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    applyTheme();
}

DashboardContactsPanel::~DashboardContactsPanel()
{
    delete m_ui;
}

void DashboardContactsPanel::applyTheme()
{
    const auto& scheme = QtMaterial::ThemeManager::instance().theme().colorScheme();
    const QColor surface = scheme.color(QtMaterial::ColorRole::Surface);
    const QColor surfaceContainer = scheme.color(QtMaterial::ColorRole::SurfaceContainerLow);
    const QColor onSurface = scheme.color(QtMaterial::ColorRole::OnSurface);
    const QColor onSurfaceVariant = scheme.color(QtMaterial::ColorRole::OnSurfaceVariant);
    const QColor primaryContainer = scheme.color(QtMaterial::ColorRole::PrimaryContainer);
    const QColor onPrimaryContainer = scheme.color(QtMaterial::ColorRole::OnPrimaryContainer);
    const QColor tertiary = scheme.color(QtMaterial::ColorRole::Tertiary);
    const QColor error = scheme.color(QtMaterial::ColorRole::Error);
    const QColor outline = scheme.color(QtMaterial::ColorRole::OutlineVariant);

    setStyleSheet(QStringLiteral(
        "#dashboardContactsPanel { background:%1; border-left:1px solid %5; }"
        "#dashboardContactsPanel QLabel { color:%2; }"
        "#dashboardContactsPanel #contactActivity { color:%3; }"
        "#dashboardContactsPanel #contactRow { background:%4; border:1px solid %5; border-radius:12px; }"
        "#dashboardContactsPanel #contactAvatar { background:%6; color:%7; border-radius:24px; font-weight:700; }"
        "#dashboardContactsPanel #panelCloseButton { background:transparent; color:%3; border:0; border-radius:18px; font-size:24px; }"
        "#dashboardContactsPanel #panelCloseButton:hover { background:%4; }"
        "QScrollArea { background:%1; border:0; }"
        "QScrollArea > QWidget > QWidget { background:%1; }")
        .arg(cssColor(surface))
        .arg(cssColor(onSurface))
        .arg(cssColor(onSurfaceVariant))
        .arg(cssColor(surfaceContainer))
        .arg(cssColor(outline))
        .arg(cssColor(primaryContainer))
        .arg(cssColor(onPrimaryContainer)));

    const auto dots = findChildren<QLabel*>(QStringLiteral("contactStatus"));
    for (QLabel* dot : dots) {
        const QString kind = dot->property("contactStatusKind").toString();
        QColor color = onSurfaceVariant;
        if (kind == QStringLiteral("online")) {
            color = tertiary;
        } else if (kind == QStringLiteral("busy")) {
            color = error;
        }
        dot->setStyleSheet(QStringLiteral("color:%1; font-size:16px;").arg(cssColor(color)));
    }

    QPalette panelPalette = palette();
    panelPalette.setColor(QPalette::Window, surface);
    panelPalette.setColor(QPalette::WindowText, onSurface);
    setPalette(panelPalette);
    setAutoFillBackground(true);
}
