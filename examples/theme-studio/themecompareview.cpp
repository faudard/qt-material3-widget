#include "themecompareview.h"

#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include "qtmaterial/theme/qtmaterialtheme.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

namespace {

void setChipStyle(QFrame* chip, const QColor& fill, const QColor& border)
{
    chip->setFixedSize(84, 40);
    chip->setStyleSheet(QStringLiteral(
        "QFrame { background-color: %1; border: 1px solid %2; border-radius: 10px; }")
        .arg(fill.name(), border.name()));
}

} // namespace

ThemeCompareView::ThemeCompareView(QWidget* parent)
    : QWidget(parent)
{
    auto* root = new QHBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(16);

    root->addWidget(createPane(tr("Light"),
                               &m_lightTitle,
                               &m_lightPrimaryChip,
                               &m_lightSurfaceChip,
                               &m_lightErrorChip));
    root->addWidget(createPane(tr("Dark"),
                               &m_darkTitle,
                               &m_darkPrimaryChip,
                               &m_darkSurfaceChip,
                               &m_darkErrorChip));
}

QWidget* ThemeCompareView::createPane(const QString& title,
                                      QLabel** titleLabel,
                                      QFrame** primaryChip,
                                      QFrame** surfaceChip,
                                      QFrame** errorChip)
{
    auto* pane = new QFrame(this);
    auto* layout = new QVBoxLayout(pane);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);

    *titleLabel = new QLabel(title, pane);
    auto* description = new QLabel(tr("Primary / Surface / Error overview"), pane);

    auto* grid = new QGridLayout();
    *primaryChip = new QFrame(pane);
    *surfaceChip = new QFrame(pane);
    *errorChip = new QFrame(pane);

    grid->addWidget(new QLabel(tr("Primary"), pane), 0, 0);
    grid->addWidget(*primaryChip, 0, 1);
    grid->addWidget(new QLabel(tr("Surface"), pane), 1, 0);
    grid->addWidget(*surfaceChip, 1, 1);
    grid->addWidget(new QLabel(tr("Error"), pane), 2, 0);
    grid->addWidget(*errorChip, 2, 1);

    layout->addWidget(*titleLabel);
    layout->addWidget(description);
    layout->addLayout(grid);
    layout->addStretch(1);

    return pane;
}

void ThemeCompareView::setBaseOptions(const ThemeOptions& options)
{
    m_baseOptions = options;

    ThemeBuilder builder;

    ThemeOptions light = m_baseOptions;
    light.mode = ThemeMode::Light;

    ThemeOptions dark = m_baseOptions;
    dark.mode = ThemeMode::Dark;

    applyPaneStyles(builder.build(light),
                    m_lightTitle,
                    m_lightPrimaryChip,
                    m_lightSurfaceChip,
                    m_lightErrorChip);

    applyPaneStyles(builder.build(dark),
                    m_darkTitle,
                    m_darkPrimaryChip,
                    m_darkSurfaceChip,
                    m_darkErrorChip);
}

void ThemeCompareView::applyTheme(const Theme& theme)
{
    setBaseOptions(theme.options());
}

void ThemeCompareView::applyPaneStyles(const Theme& theme,
                                       QLabel* titleLabel,
                                       QFrame* primaryChip,
                                       QFrame* surfaceChip,
                                       QFrame* errorChip)
{
    const auto& scheme = theme.colorScheme();

    const QColor surface = scheme.color(ColorRole::Surface);
    const QColor onSurface = scheme.color(ColorRole::OnSurface);
    const QColor outline = scheme.color(ColorRole::OutlineVariant);

    titleLabel->setStyleSheet(QStringLiteral("QLabel { color: %1; font-weight: 600; }")
                                  .arg(onSurface.name()));

    if (auto* pane = qobject_cast<QFrame*>(titleLabel->parentWidget())) {
        pane->setStyleSheet(QStringLiteral(
            "QFrame { background-color: %1; border: 1px solid %2; border-radius: 16px; }")
            .arg(surface.name(), outline.name()));
    }

    setChipStyle(primaryChip, scheme.color(ColorRole::Primary), outline);
    setChipStyle(surfaceChip, scheme.color(ColorRole::SurfaceContainerHigh), outline);
    setChipStyle(errorChip, scheme.color(ColorRole::Error), outline);
}
