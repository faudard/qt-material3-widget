# C++ API reference

This section is generated from the public headers with Doxygen and rendered in Sphinx with Breathe.

At the moment, the public headers are only lightly annotated. The generated pages are therefore best treated as a **structural API map** until richer inline Doxygen comments are added.

## Theme API

```{eval-rst}
.. doxygenclass:: QtMaterial::ThemeOptions
   :project: qtmaterial3
   :members:
```

```{eval-rst}
.. doxygenclass:: QtMaterial::ColorScheme
   :project: qtmaterial3
   :members:
```

```{eval-rst}
.. doxygenclass:: QtMaterial::TypographyScale
   :project: qtmaterial3
   :members:
```

```{eval-rst}
.. doxygenclass:: QtMaterial::ShapeScale
   :project: qtmaterial3
   :members:
```

```{eval-rst}
.. doxygenclass:: QtMaterial::ElevationScale
   :project: qtmaterial3
   :members:
```

```{eval-rst}
.. doxygenclass:: QtMaterial::MotionTokens
   :project: qtmaterial3
   :members:
```

```{eval-rst}
.. doxygenclass:: QtMaterial::StateLayer
   :project: qtmaterial3
   :members:
```

```{eval-rst}
.. doxygenclass:: QtMaterial::Theme
   :project: qtmaterial3
   :members:
```

```{eval-rst}
.. doxygenclass:: QtMaterial::ThemeBuilder
   :project: qtmaterial3
   :members:
```

```{eval-rst}
.. doxygenclass:: QtMaterial::ThemeManager
   :project: qtmaterial3
   :members:
```

## Core widget API

```{eval-rst}
.. doxygenclass:: QtMaterial::QtMaterialControl
   :project: qtmaterial3
   :members:
```

```{eval-rst}
.. doxygenclass:: QtMaterial::QtMaterialAbstractButton
   :project: qtmaterial3
   :members:
```

## Buttons API

```{eval-rst}
.. doxygenclass:: QtMaterial::QtMaterialTextButton
   :project: qtmaterial3
   :members:
```

```{eval-rst}
.. doxygenclass:: QtMaterial::QtMaterialFilledButton
   :project: qtmaterial3
   :members:
```

```{eval-rst}
.. doxygenclass:: QtMaterial::QtMaterialIconButton
   :project: qtmaterial3
   :members:
```

## Guidance for maintainers

When you add new public classes, expose them here only after:

1. the header is installed from `include/`
2. the class is part of the supported public API
3. the header has at least a short Doxygen brief explaining purpose and ownership
