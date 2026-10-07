# Native Qt bridge

Real Qt Widgets applications often mix Material widgets with native Qt widgets such as `QPushButton`, `QLineEdit`, `QTableView`, `QTreeView`, `QMenu` and `QDialog`.

This library exposes the native Qt bridge through the `Integration` module:

```cpp
#include <qtmaterial/integration/qtmaterialpaletteadapter.h>
#include <qtmaterial/integration/qtmaterialproxystyle.h>


QApplication::setPalette(QtMaterial::QtMaterialPaletteAdapter::toPalette(theme));
QApplication::setStyle(new QtMaterial::QtMaterialProxyStyle(theme));
```

The bridge does not turn native widgets into Material components. Its purpose is to keep mixed applications visually coherent.

Release rule: custom Material widgets should continue to consume resolved specs. The palette/style bridge is for native Qt integration only.


## Progressive component adaptation

The Widgets module complements the bridge with a narrow, opt-in component
adapter for existing `QPushButton` instances:

```cpp
#include <qtmaterial/widgets/native/qtmaterialbuttonadapter.h>

QtMaterial::QtMaterialButtonAdapter::apply(
    button,
    QtMaterial::ButtonVariant::Filled);
```

This does not change the widget's C++ type. The adapter installs a per-widget
proxy style, resolves the canonical button specs through the effective
`ThemeContext`, and can restore the previous style. This is the preferred
migration boundary for legacy/native forms; it must not duplicate the full
Material widget interaction implementation.

The architecture intentionally separates native adapters for incremental
migration from first-class `QtMaterial*` components that own the complete
Material interaction, ripple, motion and Expressive behavior.
