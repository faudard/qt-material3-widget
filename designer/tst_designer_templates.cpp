
#include <QtTest/QTest>

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTableView>
#include <QTreeView>
#include <QWidget>

#include "qtmaterial/widgets/native/qtmaterialnativeadapter.h"
#include "ui_settings.h"
#include "ui_login.h"
#include "ui_dashboard.h"
#include "ui_data_explorer.h"
#include "ui_responsive_navigation.h"

using QtMaterial::QtMaterialNativeAdapter;

class Designer4TemplatesTest : public QObject
{
    Q_OBJECT
private slots:
    void settingsRoundTrip()
    {
        QWidget root;
        Ui::Designer4Settings ui;
        ui.setupUi(&root);
        QCOMPARE(ui.topBar->title(), QStringLiteral("Settings"));
        QVERIFY(ui.notifications->isChecked());
        QCOMPARE(ui.locale->count(), 2);
        QCOMPARE(ui.saveSettings->property("qtm3MaterialVariant").toString(),
                 QStringLiteral("filled-tonal"));
        QCOMPARE(ui.saveSettings->property("qtm3MaterialDensity").toString(),
                 QStringLiteral("compact"));
        QCOMPARE(QString::fromLatin1(ui.saveSettings->metaObject()->className()),
                 QStringLiteral("QPushButton"));
        QVERIFY(QtMaterialNativeAdapter::applyDeclaredToDescendants(&root) > 0);
        QVERIFY(QtMaterialNativeAdapter::isApplied(ui.saveSettings));
        QVERIFY(QtMaterialNativeAdapter::removeFromDescendants(&root) > 0);
    }

    void loginRoundTrip()
    {
        QWidget root;
        Ui::Designer4Login ui;
        ui.setupUi(&root);
        QCOMPARE(ui.accountName->placeholderText(), QStringLiteral("name@example.com"));
        QCOMPARE(ui.password->echoMode(), QLineEdit::Password);
        QCOMPARE(ui.password->property("qtm3MaterialTextFieldVariant").toString(),
                 QStringLiteral("outlined"));
        QCOMPARE(ui.signIn->property("qtm3MaterialVariant").toString(),
                 QStringLiteral("filled"));
        QVERIFY(QtMaterialNativeAdapter::applyDeclaredToDescendants(&root) > 0);
        QVERIFY(QtMaterialNativeAdapter::isApplied(ui.password));
        QVERIFY(QtMaterialNativeAdapter::removeFromDescendants(&root) > 0);
    }

    void dashboardRoundTrip()
    {
        QWidget root;
        Ui::Designer4Dashboard ui;
        ui.setupUi(&root);
        QCOMPARE(ui.kpiCard->titleText(), QStringLiteral("Weekly activity"));
        QCOMPARE(ui.progress->value(), 64);
        QCOMPARE(ui.exportButton->text(), QStringLiteral("Export"));
        QCOMPARE(ui.refreshData->property("qtm3MaterialVariant").toString(),
                 QStringLiteral("outlined"));
    }

    void dataExplorerRoundTrip()
    {
        QWidget root;
        Ui::Designer4DataExplorer ui;
        ui.setupUi(&root);
        QCOMPARE(QString::fromLatin1(ui.folders->metaObject()->className()),
                 QStringLiteral("QTreeView"));
        QCOMPARE(QString::fromLatin1(ui.records->metaObject()->className()),
                 QStringLiteral("QTableView"));
        QCOMPARE(ui.searchRecords->property("qtm3MaterialTextFieldVariant").toString(),
                 QStringLiteral("filled"));
        QVERIFY(QtMaterialNativeAdapter::applyDeclaredToDescendants(&root) > 0);
        QVERIFY(QtMaterialNativeAdapter::isApplied(ui.searchRecords));
        QVERIFY(QtMaterialNativeAdapter::removeFromDescendants(&root) > 0);
    }

    void responsiveNavigationRoundTrip()
    {
        QWidget root;
        Ui::Designer4ResponsiveNavigation ui;
        ui.setupUi(&root);
        QCOMPARE(ui.navigation->destinationLabels(),
                 QStringList({QStringLiteral("Home"),
                              QStringLiteral("Projects"),
                              QStringLiteral("Account")}));
        QCOMPARE(ui.navigation->currentIndex(), 1);
        QCOMPARE(ui.pageTabs->count(), 2);
        QCOMPARE(ui.pageTabs->tabText(1), QStringLiteral("Details"));
        QVERIFY(!ui.adaptiveShell->automaticDensity());
        QCOMPARE(ui.adaptiveShell->supportingPaneWidth(), 320);
        // Shell slot ownership is intentionally not represented by these properties.
        QVERIFY(ui.adaptiveShell->contentWidget() == nullptr);
    }
};

QTEST_MAIN(Designer4TemplatesTest)
#include "tst_designer_templates.moc"
