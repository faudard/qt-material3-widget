#include <QtTest/QtTest>

#include <QLabel>
#include <QImage>
#include <QStackedWidget>
#include <QTabBar>
#include <QVBoxLayout>

#include "qtmaterial/widgets/navigation/qtmaterialnavigationcontroller.h"
#include "qtmaterial/widgets/navigation/qtmaterialtabs.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"

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
    void authoredSpecRoundTripsWhenThemeDisabled();
    void bindsToStackedWidget();
    void bindsToMultipleControllersBidirectionally();
    void supportsLazyLoading();
    void supportsRoutesAndUrlNavigation();
    void exposesAutomationProperties();
    void supportsKeyboardNavigation();
    void supportsPointerNavigation();
    void supportsRtlKeyboardNavigation();
    void exposesAccessibleTabBar();
    void rendersAtDesktopScaleFactors_data();
    void rendersAtDesktopScaleFactors();
    void indicatorTracksSelectionAcrossLayoutChanges_data();
    void indicatorTracksSelectionAcrossLayoutChanges();
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
    QCOMPARE(tabs.route(2).path(), QStringLiteral("/route/b"));

    tabs.removeTab(1);
    QCOMPARE(tabs.tabId(0), QStringLiteral("id.a"));
    QCOMPARE(tabs.tabId(1), QStringLiteral("id.b"));
    QCOMPARE(tabs.route(1).path(), QStringLiteral("/route/b"));
    QCOMPARE(tabs.tabTestId(1), QStringLiteral("qa.b"));
}

void TestQtMaterialTabs::authoredSpecRoundTripsWhenThemeDisabled()
{
    QtMaterial::TabsSpec spec;
    spec.useGlobalTheme = false;
    spec.variant = QtMaterial::TabsVariant::Secondary;
    spec.activeLabelColor = QColor(QStringLiteral("#123456"));
    spec.hoverOpacity = 0.22;

    QtMaterial::QtMaterialTabs tabs(spec);
    QCOMPARE(tabs.authoredSpec().variant, QtMaterial::TabsVariant::Secondary);
    QCOMPARE(tabs.spec().variant, QtMaterial::TabsVariant::Secondary);
    QCOMPARE(tabs.authoredSpec().activeLabelColor, QColor(QStringLiteral("#123456")));
    QCOMPARE(tabs.spec().activeLabelColor, QColor(QStringLiteral("#123456")));
    QCOMPARE(tabs.authoredSpec().hoverOpacity, 0.22);
    QCOMPARE(tabs.spec().hoverOpacity, 0.22);
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
    QCOMPARE(tabs.route(tabs.currentIndex()).path(), QStringLiteral("/settings/security"));
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
    QCOMPARE(page->property("materialTabRoute").toString(), QStringLiteral("/settings/profile"));
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

void TestQtMaterialTabs::supportsPointerNavigation()
{
    QtMaterial::QtMaterialTabs tabs;
    tabs.addTab(new QWidget(&tabs), QStringLiteral("Overview"));
    tabs.addTab(new QWidget(&tabs), QStringLiteral("Activity"));
    tabs.addTab(new QWidget(&tabs), QStringLiteral("Settings"));
    tabs.resize(480, 180);
    tabs.show();
    QVERIFY(QTest::qWaitForWindowExposed(&tabs));

    QTabBar* bar =
        tabs.findChild<QTabBar*>(
            QStringLiteral("qtmaterial_tabs_bar"));
    QVERIFY(bar != nullptr);

    QTest::mouseClick(
        bar,
        Qt::LeftButton,
        Qt::NoModifier,
        bar->tabRect(1).center());

    QCOMPARE(tabs.currentIndex(), 1);
}

void TestQtMaterialTabs::supportsRtlKeyboardNavigation()
{
    QWidget host;
    QVBoxLayout layout(&host);
    QtMaterial::QtMaterialTabs tabs;
    tabs.setLayoutDirection(Qt::RightToLeft);
    tabs.addTab(new QWidget(&tabs), QStringLiteral("One"));
    tabs.addTab(new QWidget(&tabs), QStringLiteral("Two"));
    tabs.addTab(new QWidget(&tabs), QStringLiteral("Three"));
    tabs.setWrapNavigation(true);
    tabs.setCurrentIndex(1);
    layout.addWidget(&tabs);

    host.show();
    QVERIFY(QTest::qWaitForWindowExposed(&host));

    QTabBar* bar = tabs.findChild<QTabBar*>(QStringLiteral("qtmaterial_tabs_bar"));
    QVERIFY(bar != nullptr);
    bar->setFocus(Qt::OtherFocusReason);
    QTRY_VERIFY(bar->hasFocus());

    QTest::keyClick(bar, Qt::Key_Right);
    QCOMPARE(tabs.currentIndex(), 0);

    QTest::keyClick(bar, Qt::Key_Left);
    QCOMPARE(tabs.currentIndex(), 1);
}

void TestQtMaterialTabs::exposesAccessibleTabBar()
{
    QtMaterial::QtMaterialTabs tabs;
    tabs.addTab(new QWidget(&tabs), QStringLiteral("Overview"));

    QTabBar* bar = tabs.findChild<QTabBar*>(QStringLiteral("qtmaterial_tabs_bar"));
    QVERIFY(bar != nullptr);
    QCOMPARE(bar->accessibleName(), QStringLiteral("Tabs"));
    QCOMPARE(bar->tabText(0), QStringLiteral("Overview"));
}

void TestQtMaterialTabs::rendersAtDesktopScaleFactors_data()
{
    QTest::addColumn<qreal>("dpr");
    QTest::newRow("100-percent") << qreal(1.00);
    QTest::newRow("125-percent") << qreal(1.25);
    QTest::newRow("150-percent") << qreal(1.50);
    QTest::newRow("175-percent") << qreal(1.75);
    QTest::newRow("200-percent") << qreal(2.00);
}

void TestQtMaterialTabs::rendersAtDesktopScaleFactors()
{
    QFETCH(qreal, dpr);
    QtMaterial::QtMaterialTabs tabs;
    tabs.resize(480, 160);
    tabs.addTab(new QWidget(&tabs), QStringLiteral("Overview"));
    tabs.addTab(new QWidget(&tabs), QStringLiteral("Settings"));

    QPixmap pixmap(
        qMax(1, qRound(tabs.width() * dpr)),
        qMax(1, qRound(tabs.height() * dpr)));
    pixmap.setDevicePixelRatio(dpr);
    pixmap.fill(Qt::transparent);
    tabs.render(&pixmap);

    QVERIFY(!pixmap.isNull());
    QCOMPARE(pixmap.devicePixelRatio(), dpr);
}

void TestQtMaterialTabs::indicatorTracksSelectionAcrossLayoutChanges_data()
{
    QTest::addColumn<int>("direction");
    QTest::addColumn<int>("duration");
    QTest::newRow("ltr-animated") << int(Qt::LeftToRight) << 10000;
    QTest::newRow("rtl-animated") << int(Qt::RightToLeft) << 10000;
    QTest::newRow("ltr-immediate") << int(Qt::LeftToRight) << 0;
    QTest::newRow("rtl-immediate") << int(Qt::RightToLeft) << 0;
}

void TestQtMaterialTabs::indicatorTracksSelectionAcrossLayoutChanges()
{
    QFETCH(int, direction);
    QFETCH(int, duration);
    QtMaterial::Theme theme = QtMaterial::ThemeBuilder().buildLightFromSeed(QColor(QStringLiteral("#6750A4")));
    QtMaterial::ThemeContext context(theme);
    QtMaterial::TabsSpec spec;
    spec.activeIndicatorColor = QColor(QStringLiteral("#FF0081"));
    spec.animationDuration = duration;
    QtMaterial::QtMaterialTabs tabs(spec);
    tabs.setThemeContext(&context);
    tabs.setLayoutDirection(Qt::LayoutDirection(direction));
    tabs.addTab(new QWidget(&tabs), QStringLiteral("First"));
    tabs.addTab(new QWidget(&tabs), QStringLiteral("Second"));
    tabs.addTab(new QWidget(&tabs), QStringLiteral("Third"));
    auto* bar = tabs.findChild<QTabBar*>();
    QVERIFY(bar);
    bar->setFocusPolicy(Qt::NoFocus);
    tabs.setFocusPolicy(Qt::NoFocus);
    const auto indicatorColor = [&]() {
        const QImage image = bar->grab().toImage();
        const QRect selected = bar->tabRect(tabs.currentIndex());
        const int y = selected.bottom() - tabs.resolvedSpec().indicatorHeight / 2;
        return image.pixelColor(qRound(selected.center().x() * image.devicePixelRatio()),
                                qRound(y * image.devicePixelRatio()));
    };

    // Hidden selection must already point at the selected tab on first show.
    tabs.setCurrentIndex(1);
    tabs.resize(480, 180);
    tabs.show();
    QVERIFY(QTest::qWaitForWindowExposed(&tabs));
    QCOMPARE(indicatorColor(), spec.activeIndicatorColor);

    // A layout change during a long animation must discard its old endpoints.
    tabs.setCurrentIndex(2);
    tabs.setLayoutDirection(direction == int(Qt::LeftToRight) ? Qt::RightToLeft : Qt::LeftToRight);
    QTest::qWait(60);
    QCOMPARE(indicatorColor(), spec.activeIndicatorColor);

    tabs.setCurrentIndex(1);
    bar->resize(bar->width() + 120, bar->height());
    QTest::qWait(60);
    QCOMPARE(indicatorColor(), spec.activeIndicatorColor);

    // A live accessibility change must finish the current selection immediately.
    tabs.setCurrentIndex(0);
    theme.accessibility().reducedMotion = true;
    QVERIFY(context.setTheme(theme));
    QCOMPARE(tabs.resolvedSpec().animationDuration, 0);
    QCOMPARE(indicatorColor(), spec.activeIndicatorColor);
    QTest::qWait(60);
    QCOMPARE(indicatorColor(), spec.activeIndicatorColor);
}

QTEST_MAIN(TestQtMaterialTabs)
#include "tst_tabs.moc"
