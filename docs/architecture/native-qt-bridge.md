# Native Qt bridge

Real Qt Widgets applications often mix Material widgets with native Qt widgets such as `QPushButton`, `QLineEdit`, `QTableView`, `QTreeView`, `QMenu` and `QDialog`.

This library now exposes a minimal bridge:

```cpp
QApplication::setPalette(QtMaterial::QtMaterialPaletteAdapter::toPalette(theme));
QApplication::setStyle(new QtMaterial::QtMaterialProxyStyle(theme));
```

The bridge does not turn native widgets into Material components. Its purpose is to keep mixed applications visually coherent.

Release rule: custom Material widgets should continue to consume resolved specs. The palette/style bridge is for native Qt integration only.
