#include "compactcontrolspage.h"

#include <QLabel>
#include <QVBoxLayout>

#include "qtmaterial/widgets/selection/qtmaterialchip.h"

using QtMaterial::ChipVariant;
using QtMaterial::QtMaterialChip;

CompactControlsPage::CompactControlsPage(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(12);

    auto* title = new QLabel(tr("0.8 — Chip family"), this);
    layout->addWidget(title);

    const struct {
        const char* label;
        ChipVariant variant;
    } variants[] = {
        {"Assist", ChipVariant::Assist},
        {"Filter", ChipVariant::Filter},
        {"Input", ChipVariant::Input},
        {"Suggestion", ChipVariant::Suggestion},
    };

    for (const auto& item : variants) {
        auto* chip = new QtMaterialChip(tr(item.label), this);
        chip->setVariant(item.variant);
        if (item.variant == ChipVariant::Input) {
            chip->setRemovable(true);
        }
        layout->addWidget(chip, 0, Qt::AlignLeft);
    }

    layout->addStretch();
}
