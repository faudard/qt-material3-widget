# QtMaterial icon system changes

## Files in this zip
- `include/qtmaterial/core/qtmaterialicon.h`
- `src/core/qtmaterialicon.cpp`
- `resources/qtmaterial_icons.qrc`
- this note

## CMake changes
Add `Svg` to your Qt components:
```cmake
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets Svg)
```

Link against Qt Svg:
```cmake
Qt${QT_VERSION_MAJOR}::Svg
```

Add these files to your explicit file lists:
```cmake
src/core/qtmaterialicon.cpp
include/qtmaterial/core/qtmaterialicon.h
resources/qtmaterial_icons.qrc
```

## Snackbar integration example
```cpp
#include "qtmaterial/core/qtmaterialicon.h"

m_dismissButton->setText(QString());
m_dismissButton->setIconSize(QSize(20, 20));
m_dismissButton->setIcon(
    QtMaterial::QtMaterialIcon::icon(
        QtMaterial::MaterialIcon::Close,
        m_specPtr->dismissIconColor,
        QSize(20, 20)));
```

## Important
The qrc file assumes your downloaded SVG files live under:
`resources/icons/material/`

If your repo layout differs, adjust the `<file>` paths inside the qrc.
