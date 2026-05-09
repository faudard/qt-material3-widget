#include "qtmaterial/widgets/navigation/qtmaterialnavigationcontroller.h"

#include <QPointer>
#include <QStackedWidget>

#include <memory>

namespace QtMaterial {

struct QtMaterialStackedWidgetControllerPrivate {
    QPointer<QStackedWidget> m_stack;
};

QtMaterialStackedWidgetController::QtMaterialStackedWidgetController(QStackedWidget* stack, QObject* parent)
    : QtMaterialNavigationController(parent)
    , d_ptr(std::make_unique<QtMaterialStackedWidgetControllerPrivate>())
{
    d_ptr->m_stack = stack;
    if (d_ptr->m_stack) {
        QObject::connect(d_ptr->m_stack,
                         SIGNAL(currentChanged(int)),
                         this,
                         SIGNAL(currentIndexChanged(int)));
    }
}

QtMaterialStackedWidgetController::~QtMaterialStackedWidgetController() = default;

int QtMaterialStackedWidgetController::currentIndex() const
{
    return d_ptr->m_stack ? d_ptr->m_stack->currentIndex() : -1;
}

void QtMaterialStackedWidgetController::setCurrentIndex(int index)
{
    if (d_ptr->m_stack) {
        d_ptr->m_stack->setCurrentIndex(index);
    }
}

QStackedWidget* QtMaterialStackedWidgetController::stackedWidget() const
{
    return d_ptr->m_stack.data();
}

} // namespace QtMaterial
