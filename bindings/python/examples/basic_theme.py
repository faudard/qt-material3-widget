from PySide6.QtGui import QColor
from PySide6.QtWidgets import QApplication, QVBoxLayout, QWidget

import QtMaterial3
from QtMaterial3 import Widgets


app = QApplication([])

options = QtMaterial3.ThemeOptions()
options.sourceColor = QColor("#6750A4")
options.variant = QtMaterial3.ThemeVariant.Expressive
options.motionScheme = QtMaterial3.MotionScheme.Expressive

theme = QtMaterial3.ThemeBuilder().build(options)
context = QtMaterial3.ThemeContext(theme)

window = QWidget()
layout = QVBoxLayout(window)

button = Widgets.QtMaterialFilledButton("PySide6 + Material 3")
button.setThemeContext(context)
button.setExpressive(True)
layout.addWidget(button)

card = Widgets.QtMaterialCard()
card.setThemeContext(context)
card.titleText = "QtMaterial3"
card.bodyText = "Native C++ widgets, generated through Shiboken6."
layout.addWidget(card)

window.resize(480, 240)
window.show()
app.exec()
