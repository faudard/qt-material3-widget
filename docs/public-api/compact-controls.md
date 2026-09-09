# Compact controls

Compact controls group dense selection and action surfaces. The current public
Chip declaration is available through:

```cpp
#include <qtmaterial/widgets/selection/qtmaterialchip.h>
```

## Current Chip boundary

`QtMaterialChip` and `ChipSpec` are implementation prototypes. The component is
listed in the registry so its API identity, documentation route and future
Material evidence cannot drift, but it is deliberately outside release scope.
It has no registered component-level widget test target and no Material
reference-model, structural, renderer or visual-conformance suite. Applications
should therefore not treat the current behavior as a stable released contract.

Before release scope can be enabled, the Chip needs focused widget tests,
explicit interaction and accessibility behavior, RTL and HiDPI coverage, and an
evidence-backed Material component model.

Segmented Button is tracked separately as `selection.segmented-button`; its
current public contract is documented in [Selection controls](selection.md).
