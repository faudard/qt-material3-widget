#include <QtTest/QtTest>

#include <QApplication>
#include <QHBoxLayout>
#include <QImage>
#include <QLineEdit>
#include <QPainter>
#include <QPushButton>
#include <QSignalSpy>
#include <QTabBar>
#include <QVBoxLayout>
#include <QtGlobal>

#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"
#include "qtmaterial/widgets/navigation/qtmaterialtabs.h"
#include "qtmaterial/widgets/progress/qtmateriallinearprogressindicator.h"
#include "qtmaterial/widgets/surfaces/qtmaterialdialog.h"

class tst_NavigationDialogHighDpiContracts : public QObject
{
    Q_OBJECT

private slots:
    void tabsSupportArrowHomeEndNavigation();
    void dialogClosesWithEscape();
    void dialogFocusesFirstFocusableBodyChild();
    void widgetsRenderAtDesktopScaleFactors_data();
    void widgetsRenderAtDesktopScaleFactors();
    void rtlSmokeForCommonInteractiveWidgets();
};

void tst_NavigationDialogHighDpiContracts::tabsSupportArrowHomeEndNavigation()
{
    QtMaterial::QtMaterialTabs tabs;
    tabs.resize(420, 240);

    tabs.addTab(new QWidget(&tabs), QStringLiteral("One"));
    tabs.addTab(new QWidget(&tabs), QStringLiteral("Two"));
    tabs.addTab(new QWidget(&tabs), QStringLiteral("Three"));
    tabs.setCurrentIndex(0);

    tabs.show();
    QVERIFY(QTest::qWaitForWindowExposed(&tabs));

    auto* tabBar = tabs.findChild<QTabBar*>();
    QVERIFY(tabBar != nullptr);
    tabBar->setFocus(Qt::OtherFocusReason);
    QTRY_VERIFY(tabBar->hasFocus());

    QTest::keyClick(tabBar, Qt::Key_Right);
    QTRY_COMPARE(tabs.currentIndex(), 1);

    QTest::keyClick(tabBar, Qt::Key_End);
    QTRY_COMPARE(tabs.currentIndex(), 2);

    QTest::keyClick(tabBar, Qt::Key_Left);
    QTRY_COMPARE(tabs.currentIndex(), 1);

    QTest::keyClick(tabBar, Qt::Key_Home);
    QTRY_COMPARE(tabs.currentIndex(), 0);
}

void tst_NavigationDialogHighDpiContracts::dialogClosesWithEscape()
{
    QtMaterial::QtMaterialDialog dialog;
    dialog.resize(360, 220);

    auto* body = new QWidget(&dialog);
    auto* layout = new QVBoxLayout(body);
    auto* field = new QLineEdit(body);
    field->setObjectName(QStringLiteral("dialog-field"));
    layout->addWidget(field);
    dialog.setBodyWidget(body);

    dialog.open();
    QVERIFY(QTest::qWaitForWindowExposed(&dialog));
    QVERIFY(dialog.isVisible());

    QTest::keyClick(&dialog, Qt::Key_Escape);
    QTRY_VERIFY(!dialog.isVisible());
}

void tst_NavigationDialogHighDpiContracts::dialogFocusesFirstFocusableBodyChild()
{
    QtMaterial::QtMaterialDialog dialog;
    dialog.resize(360, 220);

    auto* body = new QWidget(&dialog);
    auto* layout = new QVBoxLayout(body);
    auto* field = new QLineEdit(body);
    auto* button = new QPushButton(QStringLiteral("OK"), body);
    field->setObjectName(QStringLiteral("first-focusable"));
    layout->addWidget(field);
    layout->addWidget(button);
    dialog.setBodyWidget(body);

    dialog.open();
    QVERIFY(QTest::qWaitForWindowExposed(&dialog));

    QTRY_VERIFY(QApplication::focusWidget() == field || field->hasFocus());

    dialog.close();
}

void tst_NavigationDialogHighDpiContracts::widgetsRenderAtDesktopScaleFactors_data()
{
    QTest::addColumn<qreal>("dpr");

    QTest::newRow("100-percent") << qreal(1.00);
    QTest::newRow("125-percent") << qreal(1.25);
    QTest::newRow("150-percent") << qreal(1.50);
    QTest::newRow("175-percent") << qreal(1.75);
    QTest::newRow("200-percent-retina") << qreal(2.00);
}

void tst_NavigationDialogHighDpiContracts::widgetsRenderAtDesktopScaleFactors()
{
    QFETCH(qreal, dpr);
    QWidget container;
    container.resize(520, 220);
    auto* layout = new QVBoxLayout(&container);

    auto* button = new QtMaterial::QtMaterialFilledButton(&container);
    button->setText(QStringLiteral("Save"));
    button->setMaterialTestId(QStringLiteral("save-button"));

    auto* field = new QtMaterial::QtMaterialOutlinedTextField(&container);
    field->setLabelText(QStringLiteral("Name"));
    field->setSupportingText(QStringLiteral("Required"));

    auto* progress = new QtMaterial::QtMaterialLinearProgressIndicator(&container);
    progress->setMode(QtMaterial::QtMaterialLinearProgressIndicator::Mode::Determinate);
    progress->setValue(0.5);
    progress->setStatusText(QStringLiteral("Loading"));

    layout->addWidget(button);
    layout->addWidget(field);
    layout->addWidget(progress);

    container.show();
    QVERIFY(QTest::qWaitForWindowExposed(&container));

    const QSize logicalSize = container.size();
    const QSize physicalSize(
        qMax(1, qRound(logicalSize.width() * dpr)),
        qMax(1, qRound(logicalSize.height() * dpr))
            );
    QImage image(physicalSize, QImage::Format_ARGB32_Premultiplied);
    image.setDevicePixelRatio(dpr);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    container.render(&painter);
    painter.end();

    QVERIFY(!image.isNull());
    QCOMPARE(image.devicePixelRatio(), dpr);
    QCOMPARE(image.width(), physicalSize.width());
    QCOMPARE(image.height(), physicalSize.height());

    // Guard the desktop failure modes this contract is intended to catch:
    // fractional DPR must still produce a non-empty, fully rendered paint device.
    bool hasNonTransparentPixel = false;
    for (int y = 0; y < image.height() && !hasNonTransparentPixel; ++y) {
        const QRgb* scanLine = reinterpret_cast<const QRgb*>(image.constScanLine(y));
        for (int x = 0; x < image.width(); ++x) {
            if (qAlpha(scanLine[x]) != 0) {
                hasNonTransparentPixel = true;
                break;
            }
        }
    }
    QVERIFY(hasNonTransparentPixel);
}

void tst_NavigationDialogHighDpiContracts::rtlSmokeForCommonInteractiveWidgets()
{
    QWidget container;
    container.setLayoutDirection(Qt::RightToLeft);
    container.resize(520, 260);

    auto* layout = new QVBoxLayout(&container);

    auto* tabs = new QtMaterial::QtMaterialTabs(&container);
    tabs->addTab(new QWidget(tabs), QStringLiteral("الأول"));
    tabs->addTab(new QWidget(tabs), QStringLiteral("الثاني"));
    tabs->setCurrentIndex(0);

    auto* button = new QtMaterial::QtMaterialFilledButton(&container);
    button->setText(QStringLiteral("حفظ"));

    auto* field = new QtMaterial::QtMaterialOutlinedTextField(&container);
    field->setLabelText(QStringLiteral("الاسم"));

    layout->addWidget(tabs);
    layout->addWidget(button);
    layout->addWidget(field);

    container.show();
    QVERIFY(QTest::qWaitForWindowExposed(&container));

    QCOMPARE(container.layoutDirection(), Qt::RightToLeft);
    QCOMPARE(tabs->layoutDirection(), Qt::RightToLeft);
    QVERIFY(button->isVisible());
    QVERIFY(field->isVisible());
}

QTEST_MAIN(tst_NavigationDialogHighDpiContracts)
#include "tst_navigation_dialog_hidpi_contracts.moc"
