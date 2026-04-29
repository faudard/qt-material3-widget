# Progress indicators

QtMaterial3Widgets provides Material-style progress indicators for determinate and indeterminate work.

## Public headers

```cpp
#include <qtmaterial/widgets/progress/qtmateriallinearprogressindicator.h>
#include <qtmaterial/widgets/progress/qtmaterialcircularprogressindicator.h>
```

### Linear progress indicator
```cpp
auto* progress = new QtMaterial::QtMaterialLinearProgressIndicator(parent);
progress->setRange(0, 100);
progress->setValue(40);
```

### Circular progress indicator

```cpp
auto* progress = new QtMaterial::QtMaterialCircularProgressIndicator(parent);
progress->setIndeterminate(true);
Async state
```
### Progress indicators expose a common async state contract:

```cpp
progress->setBusy(true);
progress->setIndeterminate(true);
progress->setStatusText(QStringLiteral("Loading data"));
```

The async state is also reflected through automation metadata, including the materialState dynamic property.
