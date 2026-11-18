"""Native common controls and desktop SDK release-example contracts."""
import gc
import os
from pathlib import Path
import runpy
import unittest

os.environ.setdefault("QT_QPA_PLATFORM", "offscreen")

from PySide6.QtCore import Qt
from PySide6.QtWidgets import QApplication, QAbstractButton, QSlider, QWidget
from shiboken6 import Shiboken

from QtMaterial3 import ChipVariant, QtMaterialFabSize, QtMaterialFabVariant, Widgets


class CommonControls(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.app = QApplication.instance() or QApplication([])

    def test_native_classes_and_parent_lifetime(self):
        classes = (
            (Widgets.QtMaterialIconButton, QAbstractButton),
            (Widgets.QtMaterialFab, QAbstractButton),
            (Widgets.QtMaterialExtendedFab, QAbstractButton),
            (Widgets.QtMaterialSlider, QSlider),
            (Widgets.QtMaterialRangeSlider, QWidget),
            (Widgets.QtMaterialFilterChip, QAbstractButton),
            (Widgets.QtMaterialInputChip, QAbstractButton),
        )
        for cls, base in classes:
            with self.subTest(cls=cls):
                for _ in range(3):
                    parent = QWidget()
                    widget = cls(parent)
                    self.assertIsInstance(widget, base)
                    self.assertIs(widget.parent(), parent)
                    self.assertTrue(Shiboken.isValid(widget))
                    Shiboken.delete(parent)
                    self.assertFalse(Shiboken.isValid(widget))
                    del widget, parent
                    gc.collect()

    def test_buttons_and_chips(self):
        parent = QWidget()
        icon = Widgets.QtMaterialIconButton(parent)
        icon.setSelected(True)
        self.assertTrue(icon.isSelected())
        fab = Widgets.QtMaterialFab(parent)
        sizes = []
        fab.fabSizeChanged.connect(sizes.append)
        fab.setFabSize(QtMaterialFabSize.Large)
        self.assertEqual(fab.fabSize(), QtMaterialFabSize.Large)
        self.assertEqual(sizes, [QtMaterialFabSize.Large])
        fab.setFabVariant(QtMaterialFabVariant.Tertiary)
        self.assertEqual(fab.fabVariant(), QtMaterialFabVariant.Tertiary)
        extended = Widgets.QtMaterialExtendedFab("Create", parent)
        extended.setFabVariant(QtMaterialFabVariant.Secondary)
        self.assertEqual(extended.text(), "Create")
        self.assertEqual(extended.fabVariant(), QtMaterialFabVariant.Secondary)
        chip = Widgets.QtMaterialFilterChip("Filter", parent)
        self.assertEqual(chip.variant(), ChipVariant.Filter)
        chip.setChecked(True)
        self.assertTrue(chip.isChecked())
        input_chip = Widgets.QtMaterialInputChip("Recipient", parent)
        self.assertEqual(input_chip.variant(), ChipVariant.Input)
        self.assertEqual(input_chip.text(), "Recipient")
        Shiboken.delete(parent)

    def test_sliders_properties_and_signals(self):
        parent = QWidget()
        slider = Widgets.QtMaterialSlider(parent)
        visibility = []
        slider.valueLabelVisibleChanged.connect(visibility.append)
        slider.setValueLabelVisible(False)
        visibility.clear()
        slider.setValueLabelVisible(True)
        self.assertEqual(visibility, [True])
        self.assertTrue(slider.property("valueLabelVisible"))
        slider.setValue(42)
        self.assertEqual(slider.value(), 42)
        range_slider = Widgets.QtMaterialRangeSlider(parent)
        range_slider.setRange(0, 100)
        range_slider.setValues(0, 100)
        values = []
        range_slider.valuesChanged.connect(lambda low, high: values.append((low, high)))
        range_slider.setValues(20, 80)
        self.assertEqual(values[-1], (20, 80))
        self.assertEqual(range_slider.property("lowerValue"), 20)
        self.assertEqual(range_slider.property("upperValue"), 80)
        range_slider.setOrientation(Qt.Orientation.Vertical)
        self.assertEqual(range_slider.orientation(), Qt.Orientation.Vertical)
        Shiboken.delete(parent)

    def test_reference_desktop_application(self):
        path = Path(__file__).resolve().parents[1] / "examples" / "desktop_workspace.py"
        example = runpy.run_path(str(path))
        self.assertEqual(example["main"](smoke=True), 0)


if __name__ == "__main__":
    unittest.main(verbosity=2)
