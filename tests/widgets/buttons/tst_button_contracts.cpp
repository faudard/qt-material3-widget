#include <QtTest/QtTest>

#include <QAbstractButton>
#include <QAccessible>
#include <QApplication>
#include <QIcon>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QSignalSpy>
#include <QStyle>
#include <QWidget>
#include "QAccessibleInterface"

#include <functional>
#include <memory>
#include <vector>

#include "qtmaterial/widgets/buttons/qtmaterialelevatedbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialextendedfab.h"
#include "qtmaterial/widgets/buttons/qtmaterialfab.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledtonalbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialiconbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"

namespace {

struct ButtonCase
{
    QString id;
    std::function<QAbstractButton*()> create;
    bool textBearing = true;
    bool iconOnly = false;
    bool rtlRelevant = true;
    QSize minimumTouchTarget = QSize(40, 40);
};

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

void configureCommonButton(QAbstractButton* button, const ButtonCase& testCase)
{
    Q_ASSERT(button);

    button->setObjectName(testCase.id);
    button->setAccessibleName(QStringLiteral("%1 accessible action").arg(testCase.id));

    if (testCase.textBearing) {
        button->setText(QStringLiteral("Save"));
    }

    if (testCase.iconOnly || testCase.id.contains(QStringLiteral("fab"))) {
        button->setIcon(contractIcon());
    }

    if (!testCase.iconOnly && testCase.id != QStringLiteral("button.text")) {
        button->setIcon(contractIcon());
    }
}

std::vector<ButtonCase> buttonCases()
{
    using namespace QtMaterial;

    return {
        {
            QStringLiteral("button.text"),
            [] { return new QtMaterialTextButton; },
            true,
            false,
            true,
            QSize(40, 40),
        },
        {
            QStringLiteral("button.filled"),
            [] { return new QtMaterialFilledButton; },
            true,
            false,
            true,
            QSize(40, 40),
        },
        {
            QStringLiteral("button.filled-tonal"),
            [] { return new QtMaterialFilledTonalButton; },
            true,
            false,
            true,
            QSize(40, 40),
        },
        {
            QStringLiteral("button.outlined"),
            [] { return new QtMaterialOutlinedButton; },
            true,
            false,
            true,
            QSize(40, 40),
        },
        {
            QStringLiteral("button.elevated"),
            [] { return new QtMaterialElevatedButton; },
            true,
            false,
            true,
            QSize(40, 40),
        },
        {
            QStringLiteral("button.icon"),
            [] { return new QtMaterialIconButton(contractIcon()); },
            false,
            true,
            false,
            QSize(48, 48),
        },
        {
            QStringLiteral("button.fab"),
            [] { return new QtMaterialFab(contractIcon()); },
            false,
            true,
            false,
            QSize(48, 48),
        },
        {
            QStringLiteral("button.extended-fab"),
            [] { return new QtMaterialExtendedFab(contractIcon(), QStringLiteral("Create")); },
            true,
            false,
            true,
            QSize(48, 48),
        },
    };
}

QByteArray contextMessage(const ButtonCase& testCase, const char* check)
{
    return QStringLiteral("%1: %2").arg(testCase.id, QString::fromUtf8(check)).toUtf8();
}

bool imageHasPaintedPixels(const QImage& image)
{
    for (int y = 0; y < image.height(); ++y) {
        const QRgb* line = reinterpret_cast<const QRgb*>(image.constScanLine(y));
        for (int x = 0; x < image.width(); ++x) {
            if (qAlpha(line[x]) != 0) {
                return true;
            }
        }
    }
    return false;
}

} // namespace

class tst_ButtonContracts : public QObject
{
    Q_OBJECT

private slots:
    void apiBasics();
    void keyboardActivation();
    void disabledDoesNotActivate();
    void accessibilityContract();
    void iconOnlyAccessibilityPolicy();
    void sizeHintsRespectTouchTargets();
    void rtlSizeHintStability();
    void hidpiRenderSmoke();
};

void tst_ButtonContracts::apiBasics()
{
    for (const ButtonCase& testCase : buttonCases()) {
        std::unique_ptr<QAbstractButton> button(testCase.create());
        configureCommonButton(button.get(), testCase);

        QVERIFY2(button != nullptr, contextMessage(testCase, "factory returned a button").constData());
        QVERIFY2(button->isEnabled(), contextMessage(testCase, "button is enabled by default").constData());
        QCOMPARE(button->focusPolicy(), Qt::StrongFocus);
        QVERIFY2(button->sizeHint().isValid(), contextMessage(testCase, "sizeHint is valid").constData());
        QVERIFY2(button->minimumSizeHint().isValid(), contextMessage(testCase, "minimumSizeHint is valid").constData());
    }
}

void tst_ButtonContracts::keyboardActivation()
{
    for (const ButtonCase& testCase : buttonCases()) {
        std::unique_ptr<QAbstractButton> button(testCase.create());
        configureCommonButton(button.get(), testCase);

        button->resize(button->sizeHint().expandedTo(testCase.minimumTouchTarget));
        button->show();
        QVERIFY2(QTest::qWaitForWindowExposed(button.get()), contextMessage(testCase, "window exposed").constData());

        QSignalSpy clickedSpy(button.get(), &QAbstractButton::clicked);
        QVERIFY2(clickedSpy.isValid(), contextMessage(testCase, "clicked spy is valid").constData());

        button->setFocus(Qt::TabFocusReason);
        QVERIFY2(button->hasFocus(), contextMessage(testCase, "button accepts keyboard focus").constData());

        QTest::keyClick(button.get(), Qt::Key_Space);
        QVERIFY2(clickedSpy.count() >= 1, contextMessage(testCase, "Space activates button").constData());

        QTest::keyClick(button.get(), Qt::Key_Return);
        QVERIFY2(clickedSpy.count() >= 2, contextMessage(testCase, "Return activates button").constData());
    }
}

void tst_ButtonContracts::disabledDoesNotActivate()
{
    for (const ButtonCase& testCase : buttonCases()) {
        std::unique_ptr<QAbstractButton> button(testCase.create());
        configureCommonButton(button.get(), testCase);

        button->setEnabled(false);
        button->resize(button->sizeHint().expandedTo(testCase.minimumTouchTarget));
        button->show();
        QVERIFY2(QTest::qWaitForWindowExposed(button.get()), contextMessage(testCase, "window exposed").constData());

        QSignalSpy clickedSpy(button.get(), &QAbstractButton::clicked);
        QVERIFY2(clickedSpy.isValid(), contextMessage(testCase, "clicked spy is valid").constData());

        QTest::keyClick(button.get(), Qt::Key_Space);
        QTest::keyClick(button.get(), Qt::Key_Return);
        QTest::mouseClick(button.get(), Qt::LeftButton);

        QCOMPARE(clickedSpy.count(), 0);
    }
}

void tst_ButtonContracts::accessibilityContract()
{
    for (const ButtonCase& testCase : buttonCases()) {
        std::unique_ptr<QAbstractButton> button(testCase.create());
        configureCommonButton(button.get(), testCase);

        QVERIFY2(!button->accessibleName().trimmed().isEmpty(), contextMessage(testCase, "accessibleName is not empty").constData());

#if QT_CONFIG(accessibility)
        QAccessibleInterface* iface = QAccessible::queryAccessibleInterface(button.get());
        QVERIFY2(iface != nullptr, contextMessage(testCase, "QAccessible interface exists").constData());
        QCOMPARE(iface->role(), QAccessible::Button);
#endif
    }
}

void tst_ButtonContracts::iconOnlyAccessibilityPolicy()
{
    {
        QtMaterial::QtMaterialIconButton button(contractIcon());
        QVERIFY(button.requiresAccessibleName());
        QVERIFY(!button.hasUsableAccessibleName());
        button.setAccessibleName(QStringLiteral("Open document"));
        QVERIFY(button.hasUsableAccessibleName());
        QCOMPARE(button.effectiveAccessibleName(), QStringLiteral("Open document"));
    }

    {
        QtMaterial::QtMaterialFab button(contractIcon());
        QVERIFY(button.requiresAccessibleName());
        QVERIFY(!button.hasUsableAccessibleName());
        button.setIconAccessibleName(QStringLiteral("Create item"));
        QVERIFY(button.hasUsableAccessibleName());
        QCOMPARE(button.effectiveAccessibleName(), QStringLiteral("Create item"));
    }

    {
        QtMaterial::QtMaterialExtendedFab button(contractIcon(), QStringLiteral("Create"));
        QVERIFY(button.hasUsableAccessibleName());
        QCOMPARE(button.effectiveAccessibleName(), QStringLiteral("Create"));
    }
}

void tst_ButtonContracts::sizeHintsRespectTouchTargets()
{
    for (const ButtonCase& testCase : buttonCases()) {
        std::unique_ptr<QAbstractButton> button(testCase.create());
        configureCommonButton(button.get(), testCase);

        const QSize hint = button->sizeHint();
        QVERIFY2(hint.width() >= testCase.minimumTouchTarget.width(), contextMessage(testCase, "sizeHint width respects touch target").constData());
        QVERIFY2(hint.height() >= testCase.minimumTouchTarget.height(), contextMessage(testCase, "sizeHint height respects touch target").constData());
    }
}

void tst_ButtonContracts::rtlSizeHintStability()
{
    for (const ButtonCase& testCase : buttonCases()) {
        if (!testCase.rtlRelevant) {
            continue;
        }

        std::unique_ptr<QAbstractButton> button(testCase.create());
        configureCommonButton(button.get(), testCase);

        button->setLayoutDirection(Qt::LeftToRight);
        const QSize ltrHint = button->sizeHint();

        button->setLayoutDirection(Qt::RightToLeft);
        const QSize rtlHint = button->sizeHint();

        QCOMPARE(ltrHint, rtlHint);
    }
}

void tst_ButtonContracts::hidpiRenderSmoke()
{
    for (const ButtonCase& testCase : buttonCases()) {
        std::unique_ptr<QAbstractButton> button(testCase.create());
        configureCommonButton(button.get(), testCase);

        const QSize logicalSize = button->sizeHint().expandedTo(testCase.minimumTouchTarget);
        button->resize(logicalSize);

        QPixmap pixmap(logicalSize * 2);
        pixmap.setDevicePixelRatio(2.0);
        pixmap.fill(Qt::transparent);

        button->render(&pixmap);

        const QImage image = pixmap.toImage();
        QVERIFY2(!image.isNull(), contextMessage(testCase, "rendered HiDPI image is valid").constData());
        QVERIFY2(imageHasPaintedPixels(image), contextMessage(testCase, "rendered HiDPI image contains painted pixels").constData());
    }
}

QTEST_MAIN(tst_ButtonContracts)
#include "tst_button_contracts.moc"
