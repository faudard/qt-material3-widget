#include <QtTest/QTest>
#include <QSet>
#include <QWidget>
#include <QtUiPlugin/QDesignerCustomWidgetInterface>

#include "qtmaterial3designercollection.h"

class DesignerCollectionTest : public QObject
{
    Q_OBJECT

private slots:
    void exposesUniqueUsableWidgets()
    {
        QtMaterial3DesignerCollection collection;
        const auto widgets = collection.customWidgets();

        QCOMPARE(widgets.size(), 21);

        QSet<QString> names;
        for (QDesignerCustomWidgetInterface* item : widgets) {
            QVERIFY(item);
            QVERIFY(!item->name().isEmpty());
            QVERIFY(!item->group().isEmpty());
            QVERIFY(item->group().startsWith(QStringLiteral("Qt Material 3 - ")));
            QVERIFY(!item->includeFile().isEmpty());
            QVERIFY(item->domXml().contains(item->name()));
            QVERIFY(!names.contains(item->name()));
            names.insert(item->name());

            QWidget parent;
            QWidget* widget = item->createWidget(&parent);
            QVERIFY(widget);
            QCOMPARE(widget->parentWidget(), &parent);
            QVERIFY(!widget->objectName().isEmpty());
        }
    }

    void exposesExpectedGroups()
    {
        QtMaterial3DesignerCollection collection;
        QSet<QString> groups;
        for (QDesignerCustomWidgetInterface* item : collection.customWidgets())
            groups.insert(item->group());

        const QStringList expected = {
            QStringLiteral("Qt Material 3 - Buttons"),
            QStringLiteral("Qt Material 3 - Inputs"),
            QStringLiteral("Qt Material 3 - Selection"),
            QStringLiteral("Qt Material 3 - Navigation"),
            QStringLiteral("Qt Material 3 - Surfaces"),
            QStringLiteral("Qt Material 3 - Progress"),
            QStringLiteral("Qt Material 3 - Data"),
        };
        for (const QString& group : expected)
            QVERIFY2(groups.contains(group), qPrintable(group));
    }
};

QTEST_MAIN(DesignerCollectionTest)
#include "tst_designercollection.moc"
