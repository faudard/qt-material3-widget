# Compact controls

Compact controls provide dense action and selection surfaces.

## Chip family

`QtMaterialChip` is release-scoped and currently tracked as **usable**. The
0.8 contract covers the shared Chip family implementation and its Assist,
Filter, Input and Suggestion variants.

```cpp
#include <qtmaterial/widgets/selection/qtmaterialchip.h>
#include <qtmaterial/widgets/compact/qtmaterialfilterchip.h>
#include <qtmaterial/widgets/compact/qtmaterialinputchip.h>
```

The component registry records remaining work required for `complete`
maturity, including broader accessibility, HiDPI and deterministic visual
coverage. Those gaps do not make the current 0.8 API a prototype or exclude it
from release scope.

Segmented Button is tracked separately as `selection.segmented-button`; see
[Selection controls](selection.md).
