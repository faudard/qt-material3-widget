#include "qtmaterial/widgets/data/qtmaterialdivider.h"

// Insert this in the appropriate gallery/demo page.
auto *container = new QWidget;
auto *layout = new QVBoxLayout(container);
layout->setContentsMargins(24, 24, 24, 24);
layout->setSpacing(16);

auto *title = new QLabel("Dividers");
layout->addWidget(title);

layout->addWidget(new QLabel("Default horizontal divider"));
layout->addWidget(QtMaterial::QtMaterialDivider::horizontal());

layout->addWidget(new QLabel("Inset divider"));
auto *insetDivider = QtMaterial::QtMaterialDivider::horizontal();
insetDivider->setLeadingInset(24);
insetDivider->setTrailingInset(24);
layout->addWidget(insetDivider);

layout->addWidget(new QLabel("Thick divider"));
auto *thickDivider = QtMaterial::QtMaterialDivider::horizontal();
thickDivider->setThickness(3);
layout->addWidget(thickDivider);

layout->addWidget(new QLabel("Colored divider"));
auto *coloredDivider = QtMaterial::QtMaterialDivider::horizontal();
coloredDivider->setThickness(2);
coloredDivider->setColor(QColor(103, 80, 164));
layout->addWidget(coloredDivider);

auto *verticalRow = new QWidget;
auto *rowLayout = new QHBoxLayout(verticalRow);
rowLayout->setContentsMargins(0, 0, 0, 0);
rowLayout->setSpacing(16);

rowLayout->addWidget(new QLabel("Left"));

auto *verticalDivider = QtMaterial::QtMaterialDivider::vertical();
verticalDivider->setThickness(2);
verticalDivider->setLeadingInset(8);
verticalDivider->setTrailingInset(8);
rowLayout->addWidget(verticalDivider);

rowLayout->addWidget(new QLabel("Right"));

layout->addWidget(new QLabel("Vertical divider"));
layout->addWidget(verticalRow);

layout->addStretch();
