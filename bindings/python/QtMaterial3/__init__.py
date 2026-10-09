"""Qt6/PySide6 bindings for Qt Material 3 Widgets."""

# Import the Qt for Python runtime first, so libpyside6 and its Qt types are
# available even when QtMaterial3 is the first module imported in a process.
# Otherwise direct `import QtMaterial3` can fail on Linux with a missing
# libpyside6.abi3.so despite imports after PySide6 appearing to work.
from PySide6 import QtWidgets as _pyside_widgets

from . import _QtMaterial3 as _native

QtMaterial = _native.QtMaterial

Theme = QtMaterial.Theme
ThemeBuilder = QtMaterial.ThemeBuilder
ThemeContext = QtMaterial.ThemeContext
ThemeOptions = QtMaterial.ThemeOptions
ThemeMode = QtMaterial.ThemeMode
ContrastMode = QtMaterial.ContrastMode
ThemePreference = QtMaterial.ThemePreference
ThemeVariant = QtMaterial.ThemeVariant
ColorBackendPolicy = QtMaterial.ColorBackendPolicy
MotionScheme = QtMaterial.MotionScheme
Density = QtMaterial.Density
ChipVariant = QtMaterial.ChipVariant
TabsVariant = QtMaterial.TabsVariant
TabsDensity = QtMaterial.TabsDensity
TabsAlignment = QtMaterial.TabsAlignment
TabsOverflowMode = QtMaterial.TabsOverflowMode
WindowSizeClass = QtMaterial.WindowSizeClass
WindowWidthSizeClass = QtMaterial.WindowWidthSizeClass
WindowHeightSizeClass = QtMaterial.WindowHeightSizeClass

from . import Widgets

__version__ = "1.0.0"
__all__ = [
    "Widgets", "QtMaterial", "Theme", "ThemeBuilder", "ThemeContext", "ThemeOptions",
    "ThemeMode", "ContrastMode", "ThemePreference", "ThemeVariant",
    "ColorBackendPolicy", "MotionScheme", "Density", "ChipVariant", "TabsVariant", "TabsDensity",
    "TabsAlignment", "TabsOverflowMode", "WindowSizeClass",
    "WindowWidthSizeClass", "WindowHeightSizeClass",
]
