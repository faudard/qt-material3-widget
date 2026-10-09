"""Small PySide6 form using the native Material3 selection and hint widgets."""

from PySide6.QtWidgets import QApplication, QVBoxLayout, QWidget

import QtMaterial3
from QtMaterial3 import Widgets


def main():
    app = QApplication([])
    window = QWidget()
    layout = QVBoxLayout(window)

    radio = Widgets.QtMaterialRadioButton("Use expressive UI")
    radio.setChecked(True)
    layout.addWidget(radio)

    chip = Widgets.QtMaterialChip("Only favorites")
    chip.setVariant(QtMaterial3.ChipVariant.Filter)
    chip.setChecked(True)
    layout.addWidget(chip)

    tooltip = Widgets.QtMaterialTooltip(window)
    tooltip.setText("Select to show only your favorite items")
    tooltip.setTargetWidget(chip)
    tooltip.setShowDelay(250)
    tooltip.setPlacement(Widgets.QtMaterialTooltip.Placement.Below)

    window.setWindowTitle("QtMaterial3 / PySide6 selection")
    window.resize(460, 180)
    window.show()
    return app.exec()


if __name__ == "__main__":
    raise SystemExit(main())
