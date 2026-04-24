#include "themecolorrolesview.h"

#include <QClipboard>
#include <QGridLayout>
#include <QGuiApplication>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "qtmaterial/theme/qtmaterialcolorscheme.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

using namespace QtMaterial;

namespace {

QVector<ColorRole> displayedRoles()
{
    return {
        ColorRole::Primary,
        ColorRole::OnPrimary,
        ColorRole::PrimaryContainer,
        ColorRole::OnPrimaryContainer,
        ColorRole::Secondary,
        ColorRole::OnSecondary,
        ColorRole::SecondaryContainer,
        ColorRole::OnSecondaryContainer,
        ColorRole::Surface,
        ColorRole::OnSurface,
        ColorRole::SurfaceVariant,
        ColorRole::OnSurfaceVariant,
        ColorRole::Outline,
        ColorRole::OutlineVariant,
        ColorRole::Error,
        ColorRole::OnError,
        ColorRole::Background,
        ColorRole::OnBackground
    };
}

} // namespace

ThemeColorRolesView::ThemeColorRolesView(QWidget* parent)
    : QWidget(parent)
    , m_grid(new QGridLayout())
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(12);

    auto* title = new QLabel(tr("Color roles"), this);
    title->setWordWrap(true);

    root->addWidget(title);
    root->addLayout(m_grid);
    root->addStretch(1);

    m_grid->setContentsMargins(0, 0, 0, 0);
    m_grid->setHorizontalSpacing(12);
    m_grid->setVerticalSpacing(8);

    m_grid->addWidget(new QLabel(tr("Role"), this), 0, 0);
    m_grid->addWidget(new QLabel(tr("Swatch"), this), 0, 1);
    m_grid->addWidget(new QLabel(tr("Hex"), this), 0, 2);

    const QVector<ColorRole> roles = displayedRoles();
    m_rows.reserve(roles.size());

    int rowIndex = 1;
    for (ColorRole role : roles) {
        RowWidgets row;
        row.role = role;
        row.roleLabel = new QLabel(roleName(role), this);
        row.swatchButton = new QPushButton(this);
        row.hexLabel = new QLabel(this);

        row.swatchButton->setFixedSize(72, 28);
        row.swatchButton->setFlat(true);
        row.swatchButton->setCursor(Qt::PointingHandCursor);
        row.swatchButton->setToolTip(tr("Copy hex value"));
        row.hexLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

        m_grid->addWidget(row.roleLabel, rowIndex, 0);
        m_grid->addWidget(row.swatchButton, rowIndex, 1);
        m_grid->addWidget(row.hexLabel, rowIndex, 2);

        const int modelIndex = m_rows.size();
        connect(row.swatchButton, &QPushButton::clicked, this, [this, modelIndex]() {
            if (modelIndex < 0 || modelIndex >= m_rows.size()) {
                return;
            }

            const RowWidgets& row = m_rows.at(modelIndex);
            if (row.hexValue.isEmpty()) {
                return;
            }

            if (auto* clipboard = QGuiApplication::clipboard()) {
                clipboard->setText(row.hexValue);
            }

            emit colorCopied(roleName(row.role), row.hexValue);
        });

        m_rows.push_back(row);
        ++rowIndex;
    }
}

void ThemeColorRolesView::applyTheme(const Theme& theme)
{
    const auto& scheme = theme.colorScheme();

    const QColor border = scheme.contains(ColorRole::OutlineVariant)
        ? scheme.color(ColorRole::OutlineVariant)
        : QColor(QStringLiteral("#808080"));

    for (RowWidgets& row : m_rows) {
        const QColor color = scheme.contains(row.role)
            ? scheme.color(row.role)
            : QColor();

        row.hexValue = color.isValid() ? color.name(QColor::HexRgb).toUpper() : QString();
        row.hexLabel->setText(row.hexValue.isEmpty() ? tr("N/A") : row.hexValue);

        row.swatchButton->setStyleSheet(
            swatchStyle(color.isValid() ? color : QColor(Qt::transparent), border));

        row.swatchButton->setEnabled(!row.hexValue.isEmpty());
        row.swatchButton->setToolTip(
            row.hexValue.isEmpty()
                ? QStringLiteral("%1 = N/A").arg(roleName(row.role))
                : QStringLiteral("%1 = %2").arg(roleName(row.role), row.hexValue));
    }
}

QString ThemeColorRolesView::roleName(ColorRole role)
{
    switch (role) {
    case ColorRole::Primary:
        return QStringLiteral("Primary");
    case ColorRole::OnPrimary:
        return QStringLiteral("OnPrimary");
    case ColorRole::PrimaryContainer:
        return QStringLiteral("PrimaryContainer");
    case ColorRole::OnPrimaryContainer:
        return QStringLiteral("OnPrimaryContainer");
    case ColorRole::Secondary:
        return QStringLiteral("Secondary");
    case ColorRole::OnSecondary:
        return QStringLiteral("OnSecondary");
    case ColorRole::SecondaryContainer:
        return QStringLiteral("SecondaryContainer");
    case ColorRole::OnSecondaryContainer:
        return QStringLiteral("OnSecondaryContainer");
    case ColorRole::Surface:
        return QStringLiteral("Surface");
    case ColorRole::OnSurface:
        return QStringLiteral("OnSurface");
    case ColorRole::SurfaceVariant:
        return QStringLiteral("SurfaceVariant");
    case ColorRole::OnSurfaceVariant:
        return QStringLiteral("OnSurfaceVariant");
    case ColorRole::Outline:
        return QStringLiteral("Outline");
    case ColorRole::OutlineVariant:
        return QStringLiteral("OutlineVariant");
    case ColorRole::Error:
        return QStringLiteral("Error");
    case ColorRole::OnError:
        return QStringLiteral("OnError");
    case ColorRole::Background:
        return QStringLiteral("Background");
    case ColorRole::OnBackground:
        return QStringLiteral("OnBackground");
    default:
        return QStringLiteral("Unknown");
    }
}

QString ThemeColorRolesView::swatchStyle(const QColor& fill, const QColor& border)
{
    const QString fillName = fill.isValid()
        ? fill.name(QColor::HexRgb)
        : QStringLiteral("transparent");

    const QString borderName = border.isValid()
        ? border.name(QColor::HexRgb)
        : QStringLiteral("#808080");

    return QStringLiteral(
               "QPushButton {"
               " background-color: %1;"
               " border: 1px solid %2;"
               " border-radius: 8px;"
               "}")
        .arg(fillName, borderName);
}
