## QtMaterialDivider

`QtMaterialDivider` renders a lightweight separator line for layouts, forms,
menus, drawers, and list compositions.

The divider is intentionally autonomous for now: it does not instantiate a local
`Theme`. If no explicit color is set, it falls back to `QPalette::Mid`.

### Header

```cpp
#include <qtmaterial/widgets/data/qtmaterialdivider.h>
```

### Basic usage

```cpp
auto *divider = new QtMaterial::QtMaterialDivider(Qt::Horizontal);
layout->addWidget(divider);
```

### Insets

```cpp
auto *divider = QtMaterial::QtMaterialDivider::horizontal();
divider->setLeadingInset(16);
divider->setTrailingInset(16);
layout->addWidget(divider);
```

### Thickness

```cpp
auto *divider = QtMaterial::QtMaterialDivider::horizontal();
divider->setThickness(2);
layout->addWidget(divider);
```

### Explicit color

```cpp
auto *divider = QtMaterial::QtMaterialDivider::horizontal();
divider->setColor(QColor(0, 0, 0, 80));
layout->addWidget(divider);
```

### API

- `orientation()` / `setOrientation(Qt::Orientation)`
- `leadingInset()` / `setLeadingInset(int)`
- `trailingInset()` / `setTrailingInset(int)`
- `thickness()` / `setThickness(int)`
- `color()` / `setColor(const QColor&)`
- `resetColor()`
- `horizontal(QWidget*)`
- `vertical(QWidget*)`
