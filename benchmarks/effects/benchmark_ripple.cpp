#include <QtTest/QtTest>
#include <QWidget>

#include "qtmaterial/effects/qtmaterialripplecontroller.h"

class benchmark_Ripple : public QObject
{
    Q_OBJECT

private slots:
    void createAndClear_data()
    {
        QTest::addColumn<bool>("reducedMotion");
        QTest::newRow("animated") << false;
        QTest::newRow("reduced-motion") << true;
    }

    void createAndClear()
    {
        QFETCH(bool, reducedMotion);

        QWidget widget;
        widget.resize(96, 48);
        QtMaterial::QtMaterialRippleController controller(&widget);
        controller.setReducedMotion(reducedMotion);

        QBENCHMARK {
            controller.addRipple(QPointF(12.0, 12.0));
            controller.clear();
        }

        if (reducedMotion) {
            QVERIFY(!controller.isActive());
        }
    }
};

QTEST_MAIN(benchmark_Ripple)
#include "benchmark_ripple.moc"
