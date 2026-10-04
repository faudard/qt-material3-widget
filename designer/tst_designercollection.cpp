#include <QtTest/QTest>
#include <QMetaObject>
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

        QCOMPARE(widgets.size(), 27);

        QSet<QString> names;
        for (QDesignerCustomWidgetInterface* item : widgets) {
            QVERIFY(item);
            QVERIFY(!item->name().isEmpty());
            QVERIFY(!item->group().isEmpty());
            QVERIFY(item->group().startsWith(QStringLiteral("Qt Material 3 - ")));
            QVERIFY(!item->includeFile().isEmpty());
            QVERIFY(item->domXml().contains(item->name()));
            QVERIFY(item->domXml().contains(QStringLiteral("<property name=\"toolTip\">")));
            QVERIFY(!names.contains(item->name()));
            names.insert(item->name());

            QWidget parent;
            QWidget* widget = item->createWidget(&parent);
            QVERIFY(widget);
            QCOMPARE(widget->parentWidget(), &parent);
            QVERIFY(!widget->objectName().isEmpty());
        }
        QVERIFY(names.contains(QStringLiteral("QtMaterial::QtMaterialSlider")));
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

    void exposesDesignerAuthoringProperties()
    {
        QtMaterial3DesignerCollection collection;
        QHash<QString, QDesignerCustomWidgetInterface*> byName;
        for (QDesignerCustomWidgetInterface* item : collection.customWidgets())
            byName.insert(item->name(), item);

        const QStringList expectedWidgets = {
            QStringLiteral("QtMaterial::QtMaterialRangeSlider"),
            QStringLiteral("QtMaterial::QtMaterialSearchBar"),
            QStringLiteral("QtMaterialDateField"),
            QStringLiteral("QtMaterialTopAppBar"),
            QStringLiteral("QtMaterialBottomAppBar"),
            QStringLiteral("QtMaterial::QtMaterialDivider"),
        };
        for (const QString& name : expectedWidgets)
            QVERIFY2(byName.contains(name), qPrintable(name));

        QWidget parent;
        QWidget* outlined = byName.value(
            QStringLiteral("QtMaterial::QtMaterialOutlinedTextField"))->createWidget(&parent);
        QVERIFY(outlined);
        const QMetaObject* meta = outlined->metaObject();
        for (const char* property : {
                 "text",
                 "placeholderText",
                 "prefixText",
                 "suffixText",
                 "clearButtonEnabled",
                 "echoMode",
                 "readOnly",
                 "maxLength",
                 "characterCounterEnabled",
             }) {
            QVERIFY2(meta->indexOfProperty(property) >= 0, property);
        }

        const QString dateDom =
            byName.value(QStringLiteral("QtMaterialDateField"))->domXml();
        QVERIFY(dateDom.contains(QStringLiteral("displayFormat")));
        QVERIFY(dateDom.contains(QStringLiteral("clearable")));

        const QString rangeDom =
            byName.value(QStringLiteral("QtMaterial::QtMaterialRangeSlider"))->domXml();
        QVERIFY(rangeDom.contains(QStringLiteral("lowerValue")));
        QVERIFY(rangeDom.contains(QStringLiteral("upperValue")));

        const QString appBarDom =
            byName.value(QStringLiteral("QtMaterialTopAppBar"))->domXml();
        QVERIFY(appBarDom.contains(QStringLiteral("<property name=\"title\">")));
    }
};

QTEST_MAIN(DesignerCollectionTest)
#include "tst_designercollection.moc"
