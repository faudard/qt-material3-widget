#pragma once

#include <QApplication>
#include <QGuiApplication>
#include <QWidget>

// Offscreen has no desktop window manager to restore activation after popups.
// Prepare the active window, then let the tested widgets manage real Qt focus.
inline void activateTestWindow(QWidget* widget)
{
    QWidget* window = widget->window();
    window->activateWindow();
    if (QGuiApplication::platformName() == QStringLiteral("offscreen")) {
        QApplication::setActiveWindow(window);
    }
}
