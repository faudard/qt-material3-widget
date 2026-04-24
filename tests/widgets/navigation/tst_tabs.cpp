#include <QtTest/QtTest>

#include <QLabel>
#include <QStackedWidget>
#include <QTabBar>
#include <QVBoxLayout>

#include "qtmaterial/widgets/navigation/qtmaterialnavigationcontroller.h"
#include "qtmaterial/widgets/navigation/qtmaterialtabs.h"

class TestNavigationController final : public QtMaterial::QtMaterialNavigationController {
    Q_OBJECT

public:
    explicit TestNavigationController(QObject* parent = nullptr)
        : QtMaterial::QtMaterialNavigationController(parent)
    {
    }

    int currentIndex() const override { return m_currentIndex; }

public slots:
    void setCurrentIndex(int index) override
    {
        if (m_currentIndex == index) {
            return;
        }
        m_currentIndex = index;
        emit currentIndexChanged(index);
    }

private:
    int m_currentIndex = -1;
};

class TestQtMaterialTabs : public QObject {
    Q_OBJECT

private slots:
    void constructs();
    void keepsDescriptorMetadataStableAcrossInsertRemove();
    void separatesAuthoredAndResolvedSpec();
    void bindsToStackedWidget();
    void bindsToMultipleControllersBidirectionally();
    void supportsLazyLoading();
    void supportsRoutesAndUrlNavigation();
    void exposesAutomationProperties();
    void supportsKeyboardNavigation();
};

void TestQtMaterialTabs::constructs()
{
    QtMaterial::QtMaterialTabs tabs;
    QCOMPARE(tabs.count(), 0);
    QCOMPARE(tabs.variant(), QtMaterial::TabsVariant::Primary);
    QCOMPARE(tabs.density(), QtMaterial::TabsDensity::Default);
    QCOMPARE(tabs.alignment(), QtMaterial::TabsAlignment::Start);
    QCOMPARE(tabs.overflowMode(), QtMaterial::TabsOverflowMode::ScrollButtonsAndMenu);
    QVERIFY(tabs.usesGlobalTheme());
}

void TestQtMaterialTabs::keepsDescriptorMetadataStableAcrossInsertRemove()
{
    QtMaterial::QtMaterialTabs tabs;
    tabs.addTab(new QWidget(&tabs), QStringLiteral("A"));
    tabs.addTab(new QWidget(&tabs), QStringLiteral("B"));
    tabs.setTabId(0, QStringLiteral("id.a"));
    tabs.setTabId(1, QStringLiteral("id.b"));
    tabs.setRoute(0, QStringLiteral("route/a"));
    tabs.setRoute(1, QStringLiteral("route/b"));
    tabs.setTabTestId(0, QStringLiteral("qa.a"));
    tabs.setTabTestId(1, QStringLiteral("qa.b"));

    tabs.insertTab(1, new QWidget(&tabs), QStringLiteral("Inserted"));
    tabs.setTabId(1, QStringLiteral("id.inserted"));
    tabs.setRoute(1, QStringLiteral("route/inserted"));

    QCOMPARE(tabs.tabId(0), QStringLiteral("id.a"));
    QCOMPARE(tabs.tabId(1), QStringLiteral("id.inserted"));
    QCOMPARE(tabs.tabId(2), QStringLiteral("id.b"));
    QCOMPARE(tabs.route(2).path(), QStringLiteral("route/b"));

    tabs.removeTab(1);
    QCOMPARE(tabs.tabId(0), QStringLiteral("id.a"));
    QCOMPARE(tabs.tabId(1), QStringLiteral("id.b"));
    QCOMPARE(tabs.route(1).path(), QStringLiteral("route/b"));
    QCOMPARE(tabs.tabTestId(1), QStringLiteral("qa.b"));
}

void TestQtMaterialTabs::separatesAuthoredAndResolvedSpec()
{
    QtMaterial::TabsSpec spec;
    spec.useGlobalTheme = false;
    spec.variant = QtMaterial::TabsVariant::Secondary;
    spec.activeLabelColor = QColor(QStringLiteral("#123456"));
    spec.hoverOpacity = 0.22;

    QtMaterial::QtMaterialTabs tabs(spec);
    QCOMPARE(tabs.authoredSpec().variant, QtMaterial::TabsVariant::Secondary);
    QCOMPARE(tabs.resolvedSpec().variant, QtMaterial::TabsVariant::Secondary);
    QCOMPARE(tabs.authoredSpec().activeLabelColor, QColor(QStringLiteral("#123456")));
    QCOMPARE(tabs.resolvedSpec().activeLabelColor, QColor(QStringLiteral("#123456")));
    QCOMPARE(tabs.authoredSpec().hoverOpacity, 0.22);
    QCOMPARE(tabs.resolvedSpec().hoverOpacity, 0.22);
}

void TestQtMaterialTabs::bindsToStackedWidget()
{
    QtMaterial::QtMaterialTabs tabs;
    QStackedWidget stack;

    tabs.addTab(new QWidget(&tabs), QStringLiteral("One"));
    tabs.addTab(new QWidget(&tabs), QStringLiteral("Two"));
    stack.addWidget(new QWidget(&stack));
    stack.addWidget(new QWidget(&stack));

    tabs.bindTo(&stack);
    QCOMPARE(tabs.boundStackedWidget(), &stack);

    tabs.setCurrentIndex(1);
    QCOMPARE(stack.currentIndex(), 1);

    stack.setCurrentIndex(0);
    QCOMPARE(tabs.currentIndex(), 0);
}

void TestQtMaterialTabs::bindsToMultipleControllersBidirectionally()
{
    QtMaterial::QtMaterialTabs tabs;
    TestNavigationController controllerA;
    TestNavigationController controllerB;

    tabs.addTab(new QWidget(&tabs), QStringLiteral("One"));
    tabs.addTab(new QWidget(&tabs), QStringLiteral("Two"));
    tabs.addTab(new QWidget(&tabs), QStringLiteral("Three"));

    tabs.bindToController(&controllerA);
    tabs.bindToController(&controllerB);

    controllerA.setCurrentIndex(2);
    QCOMPARE(tabs.currentIndex(), 2);
    QCOMPARE(controllerB.currentIndex(), 2);

    tabs.setCurrentIndex(1);
    QCOMPARE(controllerA.currentIndex(), 1);
    QCOMPARE(controllerB.currentIndex(), 1);
}

void TestQtMaterialTabs::supportsLazyLoading()
{
    QtMaterial::QtMaterialTabs tabs;
    tabs.addTab(new QWidget(&tabs), QStringLiteral("Overview"));
    tabs.addTab(new QWidget(&tabs), QStringLiteral("Logs"));
    tabs.setRoute(1, QStringLiteral("settings/logs"));

    int factoryCalls = 0;
    tabs.setLazyLoading(true);
    tabs.setTabFactory(1, [&factoryCalls]() {
        ++factoryCalls;
        auto* content = new QLabel(QStringLiteral("Loaded"));
        content->setObjectName(QStringLiteral("logs_content"));
        return content;
    });

    QVERIFY(!tabs.isTabLoaded(1));
    QVERIFY(tabs.navigateTo(QStringLiteral("settings/logs")));
    QCOMPARE(factoryCalls, 1);
    QVERIFY(tabs.isTabLoaded(1));
    QVERIFY(tabs.widget(1)->findChild<QLabel*>(QStringLiteral("logs_content")) != nullptr);
}

void TestQtMaterialTabs::supportsRoutesAndUrlNavigation()
{
    QtMaterial::QtMaterialTabs tabs;
    tabs.addTab(new QWidget(&tabs), QStringLiteral("Profile"));
    tabs.addTab(new QWidget(&tabs), QStringLiteral("Security"));
    tabs.setRoute(0, QStringLiteral("settings/profile"));
    tabs.setRoute(1, QStringLiteral("settings/security"));

    QVERIFY(tabs.navigateToUrl(QUrl(QStringLiteral("app://settings/security"))));
    QCOMPARE(tabs.currentIndex(), 1);
    QCOMPARE(tabs.route(tabs.currentIndex()).path(), QStringLiteral("settings/security"));
}

void TestQtMaterialTabs::exposesAutomationProperties()
{
    QtMaterial::QtMaterialTabs tabs;
    tabs.addTab(new QWidget(&tabs), QStringLiteral("Profile"));
    tabs.setTabId(0, QStringLiteral("settings.profile"));
    tabs.setTabTestId(0, QStringLiteral("settings.profile.tab"));
    tabs.setRoute(0, QStringLiteral("settings/profile"));

    QWidget* page = tabs.widget(0);
    QVERIFY(page != nullptr);
    QCOMPARE(page->property("materialTabId").toString(), QStringLiteral("settings.profile"));
    QCOMPARE(page->property("materialTabTestId").toString(), QStringLiteral("settings.profile.tab"));
    QCOMPARE(page->property("materialTabRoute").toString(), QStringLiteral("settings/profile"));
    QVERIFY(tabs.findChild<QTabBar*>(QStringLiteral("qtmaterial_tabs_bar")) != nullptr);
}

void TestQtMaterialTabs::supportsKeyboardNavigation()
{
    QtMaterial::QtMaterialTabs tabs;
    tabs.addTab(new QWidget(&tabs), QStringLiteral("One"));
    tabs.addTab(new QWidget(&tabs), QStringLiteral("Two"));
    tabs.addTab(new QWidget(&tabs), QStringLiteral("Three"));
    tabs.setWrapNavigation(true);
    tabs.setCurrentIndex(0);
    tabs.setTabEnabled(1, false);

    QTabBar* bar = tabs.findChild<QTabBar*>();
    QVERIFY(bar != nullptr);
    bar->setFocus();

    QTest::keyClick(bar, Qt::Key_Right);
    QCOMPARE(tabs.currentIndex(), 2);

    QTest::keyClick(bar, Qt::Key_Right);
    QCOMPARE(tabs.currentIndex(), 0);

    QTest::keyClick(bar, Qt::Key_End);
    QCOMPARE(tabs.currentIndex(), 2);
}

QTEST_MAIN(TestQtMaterialTabs)
#include "tst_tabs.moc"
