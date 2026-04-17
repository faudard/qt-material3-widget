#include <QtTest/QtTest>
#include <QWidget>
#include "qtmaterial/effects/qtmaterialripplecontroller.h"

class benchmark_Ripple : public QObject
{
    Q_OBJECT
private slots:
    void createAndClear()
    {
        QWidget widget;
        QtMaterialRippleController controller(&widget);
        QBENCHMARK {
            controller.addRipple(QPointF(12.0, 12.0));
            controller.clear();
        }
    }
};

QTEST_MAIN(benchmark_Ripple)
#include "benchmark_ripple.moc"
