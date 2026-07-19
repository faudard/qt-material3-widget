#include "qtmaterialthemecontextpropagation_p.h"

#include <QApplication>
#include <QWidget>

namespace QtMaterial {
namespace ThemeContextPropagation {

QEvent::Type eventType() noexcept
{
    static const int registeredType = QEvent::registerEventType();
    return static_cast<QEvent::Type>(registeredType);
}

void notifyDescendants(QWidget* root)
{
    if (!root) {
        return;
    }

    const auto descendants =
        root->findChildren<QWidget*>(QString(), Qt::FindChildrenRecursively);
    for (QWidget* descendant : descendants) {
        if (!descendant) {
            continue;
        }
        QEvent event(eventType());
        QApplication::sendEvent(descendant, &event);
    }
}

} // namespace ThemeContextPropagation
} // namespace QtMaterial
