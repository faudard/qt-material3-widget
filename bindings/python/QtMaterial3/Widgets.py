"""Native widget aliases exported by the QtMaterial3 PySide6 SDK."""

from . import QtMaterial

QtMaterialTextButton = QtMaterial.QtMaterialTextButton
QtMaterialFilledButton = QtMaterial.QtMaterialFilledButton
QtMaterialOutlinedButton = QtMaterial.QtMaterialOutlinedButton
QtMaterialFilledTonalButton = QtMaterial.QtMaterialFilledTonalButton
QtMaterialElevatedButton = QtMaterial.QtMaterialElevatedButton
QtMaterialCheckbox = QtMaterial.QtMaterialCheckbox
QtMaterialRadioButton = QtMaterial.QtMaterialRadioButton
QtMaterialChip = QtMaterial.QtMaterialChip
QtMaterialTooltip = QtMaterial.QtMaterialTooltip
QtMaterialSwitch = QtMaterial.QtMaterialSwitch
QtMaterialOutlinedTextField = QtMaterial.QtMaterialOutlinedTextField
QtMaterialFilledTextField = QtMaterial.QtMaterialFilledTextField
QtMaterialCard = QtMaterial.QtMaterialCard
QtMaterialDialog = QtMaterial.QtMaterialDialog
QtMaterialSnackbar = QtMaterial.QtMaterialSnackbar
QtMaterialBadge = QtMaterial.QtMaterialBadge
QtMaterialPagination = QtMaterial.QtMaterialPagination
QtMaterialDivider = QtMaterial.QtMaterialDivider
QtMaterialNavigationBar = QtMaterial.QtMaterialNavigationBar
QtMaterialNavigationRail = QtMaterial.QtMaterialNavigationRail
QtMaterialNavigationSuite = QtMaterial.QtMaterialNavigationSuite
QtMaterialBreadcrumb = QtMaterial.QtMaterialBreadcrumb
QtMaterialTable = QtMaterial.QtMaterialTable
QtMaterialTreeView = QtMaterial.QtMaterialTreeView
QtMaterialTabs = QtMaterial.QtMaterialTabs
from . import _QtMaterial3 as _native
QtMaterialMenu = _native.QtMaterialMenu
QtMaterialLinearProgressIndicator = QtMaterial.QtMaterialLinearProgressIndicator
QtMaterialCircularProgressIndicator = QtMaterial.QtMaterialCircularProgressIndicator
QtMaterialLoadingIndicator = QtMaterial.QtMaterialLoadingIndicator

QtMaterialIconButton = QtMaterial.QtMaterialIconButton
QtMaterialFab = QtMaterial.QtMaterialFab
QtMaterialExtendedFab = QtMaterial.QtMaterialExtendedFab
QtMaterialSlider = QtMaterial.QtMaterialSlider
QtMaterialRangeSlider = QtMaterial.QtMaterialRangeSlider
QtMaterialFilterChip = QtMaterial.QtMaterialFilterChip
QtMaterialInputChip = QtMaterial.QtMaterialInputChip

__all__ = [name for name in globals() if name.startswith("QtMaterial")]
