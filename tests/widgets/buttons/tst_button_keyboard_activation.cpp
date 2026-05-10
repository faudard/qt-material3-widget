#include <QtTest/QtTest>

#include <QAbstractButton>
#include <QIcon>
#include <QKeyEvent>
#include <QPainter>
#include <QPixmap>
#include <QSignalSpy>
#include <memory>

#include "qtmaterial/widgets/buttons/qtmaterialelevatedbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialextendedfab.h"
#include "qtmaterial/widgets/buttons/qtmaterialfab.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledtonalbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialiconbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"

namespace {

QIcon contractIcon()
{
    QPixmap pixmap(24, 24);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.drawEllipse(QRectF(4, 4, 16, 16));
    painter.end();

    return QIcon(pixmap);
}

struct ButtonCase
{
    QString name;
    std::function<std::unique_ptr<QAbstractButton>()> create;
};

QVector<ButtonCase> buttonCases()
{
    return {
        {QStringLiteral("text"), [] {
             auto button = std::make_unique<QtMaterial::QtMaterialTextButton>();
             button->setText(QStringLiteral("Text"));
             return button;
         }},
        {QStringLiteral("filled"), [] {
             auto button = std::make_unique<QtMaterial::QtMaterialFilledButton>();
             button->setText(QStringLiteral("Filled"));
             return button;
         }},
        {QStringLiteral("filled-tonal"), [] {
             auto button = std::make_unique<QtMaterial::QtMaterialFilledTonalButton>(QStringLiteral("Tonal"));
             return button;
         }},
        {QStringLiteral("outlined"), [] {
             auto button = std::make_unique<QtMaterial::QtMaterialOutlinedButton>();
             button->setText(QStringLiteral("Outlined"));
             return button;
         }},
        {QStringLiteral("elevated"), [] {
             auto button = std::make_unique<QtMaterial::QtMaterialElevatedButton>();
             button->setText(QStringLiteral("Elevated"));
             return button;
         }},
        {QStringLiteral("icon"), [] {
             auto button = std::make_unique<QtMaterial::QtMaterialIconButton>(contractIcon());
             button->setAccessibleName(QStringLiteral("Search"));
             return button;
         }},
        {QStringLiteral("fab"), [] {
             auto button = std::make_unique<QtMaterial::QtMaterialFab>(contractIcon());
             button->setAccessibleName(QStringLiteral("Create"));
             return button;
         }},
        {QStringLiteral("extended-fab"), [] {
             auto button = std::make_unique<QtMaterial::QtMaterialExtendedFab>(contractIcon(), QStringLiteral("Create"));
             return button;
         }},
    };
}

void prepareButton(QAbstractButton& button)
{
    button.resize(button.sizeHint().expandedTo(QSize(128, 56)));
    button.show();
    QTest::qWaitForWindowExposed(&button);
    button.setFocus(Qt::TabFocusReason);
    QVERIFY(button.hasFocus());
}

} // namespace

class tst_ButtonKeyboardActivation : public QObject
{
    Q_OBJECT

private slots:
    void spaceActivatesOnce();
    void returnActivatesOnce();
    void enterActivatesOnce();
    void disabledDoesNotActivateFromKeyboard();
    void keyEventsLeaveButtonReleased();
};

void tst_ButtonKeyboardActivation::spaceActivatesOnce()
{
    for (const ButtonCase& testCase : buttonCases()) {
        auto button = testCase.create();
        prepareButton(*button);

        QSignalSpy clickedSpy(button.get(), &QAbstractButton::clicked);
        QVERIFY2(clickedSpy.isValid(), qPrintable(testCase.name));

        QTest::keyClick(button.get(), Qt::Key_Space);
        QCOMPARE(clickedSpy.count(), 1);
    }
}

void tst_ButtonKeyboardActivation::returnActivatesOnce()
{
    for (const ButtonCase& testCase : buttonCases()) {
        auto button = testCase.create();
        prepareButton(*button);

        QSignalSpy clickedSpy(button.get(), &QAbstractButton::clicked);
        QVERIFY2(clickedSpy.isValid(), qPrintable(testCase.name));

        QTest::keyClick(button.get(), Qt::Key_Return);
        QCOMPARE(clickedSpy.count(), 1);
    }
}

void tst_ButtonKeyboardActivation::enterActivatesOnce()
{
    for (const ButtonCase& testCase : buttonCases()) {
        auto button = testCase.create();
        prepareButton(*button);

        QSignalSpy clickedSpy(button.get(), &QAbstractButton::clicked);
        QVERIFY2(clickedSpy.isValid(), qPrintable(testCase.name));

        QTest::keyClick(button.get(), Qt::Key_Enter);
        QCOMPARE(clickedSpy.count(), 1);
    }
}

void tst_ButtonKeyboardActivation::disabledDoesNotActivateFromKeyboard()
{
    for (const ButtonCase& testCase : buttonCases()) {
        auto button = testCase.create();
        prepareButton(*button);
        button->setEnabled(false);

        QSignalSpy clickedSpy(button.get(), &QAbstractButton::clicked);
        QVERIFY2(clickedSpy.isValid(), qPrintable(testCase.name));

        QTest::keyClick(button.get(), Qt::Key_Space);
        QTest::keyClick(button.get(), Qt::Key_Return);
        QTest::keyClick(button.get(), Qt::Key_Enter);
        QCOMPARE(clickedSpy.count(), 0);
    }
}

void tst_ButtonKeyboardActivation::keyEventsLeaveButtonReleased()
{
    for (const ButtonCase& testCase : buttonCases()) {
        auto button = testCase.create();
        prepareButton(*button);

        QTest::keyClick(button.get(), Qt::Key_Space);
        QVERIFY2(!button->isDown(), qPrintable(testCase.name));

        QTest::keyClick(button.get(), Qt::Key_Return);
        QVERIFY2(!button->isDown(), qPrintable(testCase.name));

        QTest::keyClick(button.get(), Qt::Key_Enter);
        QVERIFY2(!button->isDown(), qPrintable(testCase.name));
    }
}

QTEST_MAIN(tst_ButtonKeyboardActivation)
#include "tst_button_keyboard_activation.moc"
