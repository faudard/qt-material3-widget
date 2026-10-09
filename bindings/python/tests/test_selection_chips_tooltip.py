"""Contracts for the incremental QtMaterial3 1.17.1 Python surface."""

import gc
import os
import unittest

os.environ.setdefault("QT_QPA_PLATFORM", "offscreen")

from PySide6.QtWidgets import QApplication, QWidget
from shiboken6 import Shiboken

import QtMaterial3
from QtMaterial3 import Widgets


class SelectionChipsTooltipBindingsTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.app = QApplication.instance() or QApplication([])

    def test_radio_button_selection_signal(self):
        radio = Widgets.QtMaterialRadioButton("Option A")
        states = []
        radio.toggled.connect(states.append)
        self.assertEqual(radio.text(), "Option A")
        self.assertFalse(radio.isChecked())
        radio.setChecked(True)
        self.assertTrue(radio.isChecked())
        self.assertEqual(states, [True])

    def test_chip_filter_variant_and_native_properties(self):
        chip = Widgets.QtMaterialChip("Filter")
        variants = []
        removals = []
        chip.variantChanged.connect(variants.append)
        chip.removableChanged.connect(removals.append)
        self.assertEqual(chip.variant(), QtMaterial3.ChipVariant.Assist)
        chip.setVariant(QtMaterial3.ChipVariant.Filter)
        self.assertEqual(chip.variant(), QtMaterial3.ChipVariant.Filter)
        self.assertTrue(chip.isCheckable())
        self.assertEqual(variants, [QtMaterial3.ChipVariant.Filter])
        chip.setChecked(True)
        self.assertTrue(chip.isChecked())
        chip.setRemovable(True)
        self.assertTrue(chip.isRemovable())
        self.assertEqual(removals, [True])
        self.assertEqual(chip.property("removable"), True)

    def test_tooltip_properties_signals_and_nonowning_target(self):
        target = QWidget()
        tooltip = Widgets.QtMaterialTooltip()
        texts, delays, placements = [], [], []
        tooltip.textChanged.connect(texts.append)
        tooltip.showDelayChanged.connect(delays.append)
        tooltip.placementChanged.connect(placements.append)
        tooltip.setText("A helpful label")
        tooltip.setShowDelay(0)
        tooltip.setPlacement(Widgets.QtMaterialTooltip.Placement.Below)
        self.assertEqual(texts, ["A helpful label"])
        self.assertEqual(delays, [0])
        self.assertEqual(
            placements, [Widgets.QtMaterialTooltip.Placement.Below]
        )
        self.assertEqual(tooltip.property("text"), "A helpful label")
        tooltip.setTargetWidget(target)
        self.assertIs(tooltip.targetWidget(), target)
        Shiboken.delete(target)
        self.assertIsNone(tooltip.targetWidget())
        self.assertTrue(Shiboken.isValid(tooltip))
        del target
        gc.collect()
        self.assertTrue(Shiboken.isValid(tooltip))
        Shiboken.delete(tooltip)

    def test_parent_ownership_for_new_widgets(self):
        parent = QWidget()
        radio = Widgets.QtMaterialRadioButton(parent)
        chip = Widgets.QtMaterialChip(parent)
        tooltip = Widgets.QtMaterialTooltip(parent)
        for widget in (radio, chip, tooltip):
            self.assertIs(widget.parent(), parent)
            self.assertTrue(Shiboken.isValid(widget))
        Shiboken.delete(parent)
        for widget in (radio, chip, tooltip):
            self.assertFalse(Shiboken.isValid(widget))
        del parent, radio, chip, tooltip
        gc.collect()


if __name__ == "__main__":
    unittest.main(verbosity=2)
