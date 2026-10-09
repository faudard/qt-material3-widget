"""Qt6/PySide6 bindings for Qt Material 3 Widgets."""

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
WindowSizeClass = QtMaterial.WindowSizeClass
WindowWidthSizeClass = QtMaterial.WindowWidthSizeClass
WindowHeightSizeClass = QtMaterial.WindowHeightSizeClass

from . import Widgets

__version__ = "1.0.0"
__all__ = [
    "Widgets", "QtMaterial", "Theme", "ThemeBuilder", "ThemeContext", "ThemeOptions",
    "ThemeMode", "ContrastMode", "ThemePreference", "ThemeVariant",
    "ColorBackendPolicy", "MotionScheme", "Density", "WindowSizeClass",
    "WindowWidthSizeClass", "WindowHeightSizeClass",
]
