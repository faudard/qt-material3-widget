#pragma once

#include <QEvent>

class QWidget;

namespace QtMaterial {
namespace ThemeContextPropagation {

QEvent::Type eventType() noexcept;
void notifyDescendants(QWidget* root);

} // namespace ThemeContextPropagation
} // namespace QtMaterial
