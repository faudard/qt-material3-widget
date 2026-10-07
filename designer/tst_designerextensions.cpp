#include <QtTest/QTest>

#include <QWidget>

#include "qtmaterial3designerextensions.h"

#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/widgets/buttons/qtmaterialbuttongroup.h"
#include "qtmaterial/widgets/buttons/qtmaterialsplitbutton.h"
#include "qtmaterial/widgets/data/qtmaterialsegmentedlist.h"
#include "qtmaterial/widgets/layouts/qtmaterialadaptiveshell.h"
#include "qtmaterial/widgets/navigation/qtmaterialnavigationbar.h"
#include "qtmaterial/widgets/navigation/qtmaterialtabs.h"

using namespace QtMaterial;

class DesignerExtensionsTest : public QObject
{
    Q_OBJECT

private slots:
    void exposesCuratedEditableProperties()
    {
        QtMaterialSplitButton split;
        const QStringList splitProperties =
            QtMaterial3Designer::editablePropertyNames(&split);
        QVERIFY(splitProperties.contains(QStringLiteral("text")));
        QVERIFY(splitProperties.contains(QStringLiteral("expressive")));
        QVERIFY(splitProperties.contains(QStringLiteral("expressiveSize")));
        QVERIFY(!splitProperties.contains(QStringLiteral("geometry")));

        QtMaterialButtonGroup group;
        QVERIFY(QtMaterial3Designer::editablePropertyNames(&group)
                    .contains(QStringLiteral("buttonLabels")));

        QtMaterialNavigationBar bar;
        QVERIFY(QtMaterial3Designer::editablePropertyNames(&bar)
                    .contains(QStringLiteral("destinationLabels")));

        QtMaterialSegmentedList list;
        QVERIFY(QtMaterial3Designer::editablePropertyNames(&list)
                    .contains(QStringLiteral("itemLabels")));
    }

    void tabsContainerRoundTripsPages()
    {
        QtMaterialTabs tabs;
        QtMaterial3Designer::TabsContainerExtension extension(&tabs);

        auto* first = new QWidget;
        first->setObjectName(QStringLiteral("overviewPage"));
        auto* second = new QWidget;
        second->setObjectName(QStringLiteral("detailsPage"));

        extension.addWidget(first);
        extension.addWidget(second);

        QCOMPARE(extension.count(), 2);
        QCOMPARE(extension.widget(0), first);
        QCOMPARE(extension.widget(1), second);
        QCOMPARE(extension.currentIndex(), 1);

        extension.setCurrentIndex(0);
        QCOMPARE(tabs.currentIndex(), 0);

        extension.remove(1);
        QCOMPARE(extension.count(), 1);
    }

    void adaptiveShellContainerOwnsTwoDesignerSlots()
    {
        QtMaterialAdaptiveShell shell;
        QtMaterial3Designer::AdaptiveShellContainerExtension extension(&shell);

        auto* content = new QWidget;
        auto* supporting = new QWidget;

        extension.addWidget(content);
        extension.addWidget(supporting);

        QCOMPARE(extension.count(), 2);
        QCOMPARE(shell.contentWidget(), content);
        QCOMPARE(shell.supportingWidget(), supporting);

        extension.remove(1);
        QCOMPARE(extension.count(), 1);
        QVERIFY(shell.supportingWidget() == nullptr);
        QCOMPARE(shell.contentWidget(), content);
    }

    void previewModesAreReversible()
    {
        ThemeManager& manager = ThemeManager::instance();
        const ThemeOptions original = manager.options();

        QtMaterial3Designer::applyPreviewMode(
            QtMaterial3Designer::PreviewMode::Dark);
        QCOMPARE(manager.options().mode, ThemeMode::Dark);
        QCOMPARE(manager.options().variant, ThemeVariant::TonalSpot);

        QtMaterial3Designer::applyPreviewMode(
            QtMaterial3Designer::PreviewMode::Expressive);
        QCOMPARE(manager.options().mode, ThemeMode::Light);
        QCOMPARE(manager.options().variant, ThemeVariant::Expressive);

        QtMaterial3Designer::restorePreviewMode();
        QVERIFY(manager.options() == original);
    }
};

QTEST_MAIN(DesignerExtensionsTest)
#include "tst_designerextensions.moc"
