#include <QtTest/QtTest>
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"

class benchmark_WidgetCreation : public QObject
{
    Q_OBJECT
private slots:
    void createFilledButton()
    {
        QBENCHMARK {
            QtMaterialFilledButton button;
            button.setText(QStringLiteral("Hello"));
        }
    }
};

QTEST_MAIN(benchmark_WidgetCreation)
#include "benchmark_widget_creation.moc"
