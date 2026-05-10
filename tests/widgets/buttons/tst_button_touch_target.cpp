#include <QtTest/QtTest>

#include <QAbstractButton>
#include <QFontMetrics>
#include <QIcon>
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

constexpr int kMinimumTouchTarget = 40;

QIcon contractIcon()
{
    QPixmap pixmap(32, 32);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.drawEllipse(QRectF(4, 4, 24, 24));
    painter.end();

    return QIcon(pixmap);
}

struct ButtonCase
{
    QString name;
    bool iconOnly;
    std::function<std::unique_ptr<QAbstractButton>()> create;
};

QVector<ButtonCase> buttonCases()
{
    return {
        {QStringLiteral("text"), false, [] {
             auto button = std::make_unique<QtMaterial::QtMaterialTextButton>();
             button->setText(QStringLiteral("Text"));
             return button;
         }},
        {QStringLiteral("filled"), false, [] {
             auto button = std::make_unique<QtMaterial::QtMaterialFilledButton>();
             button->setText(QStringLiteral("Filled"));
             return button;
         }},
        {QStringLiteral("filled-tonal"), false, [] {
             return std::make_unique<QtMaterial::QtMaterialFilledTonalButton>(QStringLiteral("Tonal"));
         }},
        {QStringLiteral("outlined"), false, [] {
             auto button = std::make_unique<QtMaterial::QtMaterialOutlinedButton>();
             button->setText(QStringLiteral("Outlined"));
             return button;
         }},
        {QStringLiteral("elevated"), false, [] {
             auto button = std::make_unique<QtMaterial::QtMaterialElevatedButton>();
             button->setText(QStringLiteral("Elevated"));
             return button;
         }},
        {QStringLiteral("icon"), true, [] {
             auto button = std::make_unique<QtMaterial::QtMaterialIconButton>(contractIcon());
             button->setAccessibleName(QStringLiteral("Search"));
             return button;
         }},
        {QStringLiteral("fab"), true, [] {
             auto button = std::make_unique<QtMaterial::QtMaterialFab>(contractIcon());
             button->setAccessibleName(QStringLiteral("Create"));
             return button;
         }},
        {QStringLiteral("extended-fab"), false, [] {
             return std::make_unique<QtMaterial::QtMaterialExtendedFab>(contractIcon(), QStringLiteral("Create"));
         }},
    };
}

void showAtHintSize(QAbstractButton& button)
{
    button.resize(button.sizeHint());
    button.show();
    QTest::qWaitForWindowExposed(&button);
}

} // namespace

class tst_ButtonTouchTarget : public QObject
{
    Q_OBJECT

private slots:
    void sizeHintsMeetMinimumTouchTarget();
    void minimumSizeHintsAreValidAndNotLargerThanSizeHint();
    void centerPointActivatesEnabledButtons();
    void centerPointDoesNotActivateDisabledButtons();
    void focusPolicyAllowsKeyboardUse();
    void iconOnlyButtonsRemainCompactAndSquareish();
    void textBearingButtonsReserveVisibleTextWidth();
};

void tst_ButtonTouchTarget::sizeHintsMeetMinimumTouchTarget()
{
    for (const ButtonCase& testCase : buttonCases()) {
        auto button = testCase.create();
        const QSize hint = button->sizeHint();

        QVERIFY2(hint.isValid(), qPrintable(testCase.name));
        QVERIFY2(hint.width() >= kMinimumTouchTarget, qPrintable(testCase.name));
        QVERIFY2(hint.height() >= kMinimumTouchTarget, qPrintable(testCase.name));
    }
}

void tst_ButtonTouchTarget::minimumSizeHintsAreValidAndNotLargerThanSizeHint()
{
    for (const ButtonCase& testCase : buttonCases()) {
        auto button = testCase.create();
        const QSize hint = button->sizeHint();
        const QSize minimum = button->minimumSizeHint();

        QVERIFY2(minimum.isValid(), qPrintable(testCase.name));
        QVERIFY2(minimum.width() <= hint.width(), qPrintable(testCase.name));
        QVERIFY2(minimum.height() <= hint.height(), qPrintable(testCase.name));
        QVERIFY2(minimum.width() >= kMinimumTouchTarget || hint.width() >= kMinimumTouchTarget, qPrintable(testCase.name));
        QVERIFY2(minimum.height() >= kMinimumTouchTarget || hint.height() >= kMinimumTouchTarget, qPrintable(testCase.name));
    }
}

void tst_ButtonTouchTarget::centerPointActivatesEnabledButtons()
{
    for (const ButtonCase& testCase : buttonCases()) {
        auto button = testCase.create();
        showAtHintSize(*button);

        QSignalSpy clickedSpy(button.get(), &QAbstractButton::clicked);
        QVERIFY2(clickedSpy.isValid(), qPrintable(testCase.name));

        QTest::mouseClick(button.get(), Qt::LeftButton, Qt::NoModifier, button->rect().center());
        QCOMPARE(clickedSpy.count(), 1);
    }
}

void tst_ButtonTouchTarget::centerPointDoesNotActivateDisabledButtons()
{
    for (const ButtonCase& testCase : buttonCases()) {
        auto button = testCase.create();
        showAtHintSize(*button);
        button->setEnabled(false);

        QSignalSpy clickedSpy(button.get(), &QAbstractButton::clicked);
        QVERIFY2(clickedSpy.isValid(), qPrintable(testCase.name));

        QTest::mouseClick(button.get(), Qt::LeftButton, Qt::NoModifier, button->rect().center());
        QCOMPARE(clickedSpy.count(), 0);
    }
}

void tst_ButtonTouchTarget::focusPolicyAllowsKeyboardUse()
{
    for (const ButtonCase& testCase : buttonCases()) {
        auto button = testCase.create();
        QVERIFY2(button->focusPolicy() != Qt::NoFocus, qPrintable(testCase.name));
    }
}

void tst_ButtonTouchTarget::iconOnlyButtonsRemainCompactAndSquareish()
{
    for (const ButtonCase& testCase : buttonCases()) {
        if (!testCase.iconOnly) {
            continue;
        }

        auto button = testCase.create();
        const QSize hint = button->sizeHint();
        QVERIFY2(qAbs(hint.width() - hint.height()) <= 8, qPrintable(testCase.name));
    }
}

void tst_ButtonTouchTarget::textBearingButtonsReserveVisibleTextWidth()
{
    for (const ButtonCase& testCase : buttonCases()) {
        if (testCase.iconOnly) {
            continue;
        }

        auto button = testCase.create();
        const QString label = button->text();
        const int textWidth = QFontMetrics(button->font()).horizontalAdvance(label);
        const QSize hint = button->sizeHint();

        QVERIFY2(hint.width() >= textWidth, qPrintable(testCase.name));
    }
}

QTEST_MAIN(tst_ButtonTouchTarget)
#include "tst_button_touch_target.moc"
