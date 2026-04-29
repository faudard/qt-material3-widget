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
    void widgetsRenderAtHighDevicePixelRatio();
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
    tabBar->setFocus();
    QVERIFY(tabBar->hasFocus());

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

void tst_NavigationDialogHighDpiContracts::widgetsRenderAtHighDevicePixelRatio()
{
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

    constexpr qreal dpr = 2.0;
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
