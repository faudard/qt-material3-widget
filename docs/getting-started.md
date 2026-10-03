# Getting started in 5 minutes

This guide takes a Qt Widgets application from zero to its first Material 3 control.

## Requirements

- Qt 5.14.2+ or Qt 6.4+
- CMake 3.21+
- C++17

## 1. Add the library

For an existing checkout:

```cmake
add_subdirectory(path/to/qt-material3-widget)

target_link_libraries(my_app PRIVATE
    QtMaterial3::ThemeModel
    QtMaterial3::ThemeRuntime
    QtMaterial3::Widgets
)
```

For other supported integration modes, see [Installation](installation.md).

## 2. Create and apply a theme

```cpp
#include <QApplication>
#include <QColor>

#include <qtmaterial/theme/qtmaterialthemebuilder.h>
#include <qtmaterial/theme/qtmaterialthememanager.h>
#include <qtmaterial/widgets/buttons/qtmaterialfilledbutton.h>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    const auto theme =
        QtMaterial::ThemeBuilder{}.buildLightFromSeed(QColor("#6750A4"));
    QtMaterial::ThemeManager::instance().setTheme(theme);

    QtMaterialFilledButton button;
    button.setText("Continue");
    button.resize(180, 48);
    button.show();

    return app.exec();
}
```

## 3. Build

```bash
cmake -S . -B build
cmake --build build
```

You now have a Material 3 widget using the application theme.

## Next steps

- [Installation](installation.md) — choose the right dependency model.
- [Themes](themes.md) — dark mode, dynamic colors and component overrides.
- [Widgets](widgets/index.md) — choose a component and inspect its contract.
- [Examples](examples/index.md) — run the gallery, dashboard and Theme Studio.
- [Migration](migration/index.md) — upgrade between 1.x releases.
