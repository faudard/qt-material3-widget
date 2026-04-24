#pragma once

#include <QObject>
#include <QPointer>
#include <qstackedwidget.h>

#include "qtmaterial/qtmaterialglobal.h"

QT_BEGIN_NAMESPACE
class QStackedWidget;
QT_END_NAMESPACE

namespace QtMaterial {

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialNavigationController : public QObject {
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)

public:
    explicit QtMaterialNavigationController(QObject* parent = nullptr) : QObject(parent) {}
    ~QtMaterialNavigationController() override = default;

    virtual int currentIndex() const = 0;

public slots:
    virtual void setCurrentIndex(int index) = 0;

signals:
    void currentIndexChanged(int index);
};

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialStackedWidgetController final : public QtMaterialNavigationController {
    Q_OBJECT

public:
    explicit QtMaterialStackedWidgetController(QStackedWidget* stack, QObject* parent = nullptr);
    ~QtMaterialStackedWidgetController() override = default;

    int currentIndex() const override;

public slots:
    void setCurrentIndex(int index) override;

public:
    QStackedWidget* stackedWidget() const;

private:
    QPointer<QStackedWidget> m_stack;
};

inline QtMaterialStackedWidgetController::QtMaterialStackedWidgetController(QStackedWidget* stack, QObject* parent)
    : QtMaterialNavigationController(parent), m_stack(stack)
{
    if (m_stack) {
        QObject::connect(m_stack, SIGNAL(currentChanged(int)), this, SIGNAL(currentIndexChanged(int)));
    }
}

inline int QtMaterialStackedWidgetController::currentIndex() const
{
    return m_stack ? m_stack->currentIndex() : -1;
}

inline void QtMaterialStackedWidgetController::setCurrentIndex(int index)
{
    if (m_stack) {
        m_stack->setCurrentIndex(index);
    }
}

inline QStackedWidget* QtMaterialStackedWidgetController::stackedWidget() const
{
    return m_stack.data();
}

} // namespace QtMaterial
