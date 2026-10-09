"""Widget aliases exported by the first QtMaterial3 PySide6 binding surface."""

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
QtMaterialTabs = QtMaterial.QtMaterialTabs
from . import _QtMaterial3 as _native
QtMaterialMenu = _native.QtMaterialMenu
QtMaterialLinearProgressIndicator = QtMaterial.QtMaterialLinearProgressIndicator
QtMaterialCircularProgressIndicator = QtMaterial.QtMaterialCircularProgressIndicator
QtMaterialLoadingIndicator = QtMaterial.QtMaterialLoadingIndicator

__all__ = [name for name in globals() if name.startswith("QtMaterial")]
