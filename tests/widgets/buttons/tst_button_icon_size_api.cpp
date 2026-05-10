#include <QtTest/QtTest>

#include <QAbstractButton>
#include <QIcon>
#include <QPainter>
#include <QPixmap>
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
    std::function<std::unique_ptr<QAbstractButton>()> create;
};

QVector<ButtonCase> buttonCases()
{
    return {
        {QStringLiteral("text"), [] {
             auto button = std::make_unique<QtMaterial::QtMaterialTextButton>();
             button->setText(QStringLiteral("Text"));
             button->setIcon(contractIcon());
             return button;
         }},
        {QStringLiteral("filled"), [] {
             auto button = std::make_unique<QtMaterial::QtMaterialFilledButton>();
             button->setText(QStringLiteral("Filled"));
             button->setIcon(contractIcon());
             return button;
         }},
        {QStringLiteral("filled-tonal"), [] {
             auto button = std::make_unique<QtMaterial::QtMaterialFilledTonalButton>(QStringLiteral("Tonal"));
             button->setIcon(contractIcon());
             return button;
         }},
        {QStringLiteral("outlined"), [] {
             auto button = std::make_unique<QtMaterial::QtMaterialOutlinedButton>();
             button->setText(QStringLiteral("Outlined"));
             button->setIcon(contractIcon());
             return button;
         }},
        {QStringLiteral("elevated"), [] {
             auto button = std::make_unique<QtMaterial::QtMaterialElevatedButton>();
             button->setText(QStringLiteral("Elevated"));
             button->setIcon(contractIcon());
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
             return std::make_unique<QtMaterial::QtMaterialExtendedFab>(contractIcon(), QStringLiteral("Create"));
         }},
    };
}

} // namespace

class tst_ButtonIconSizeApi : public QObject
{
    Q_OBJECT

private slots:
    void setIconSizeStoresRequestedSize();
    void setIconSizeKeepsSizeHintValid();
    void setIconSizeKeepsRenderingValid();
    void repeatedSetIconSizeIsStable();
};

void tst_ButtonIconSizeApi::setIconSizeStoresRequestedSize()
{
    for (const ButtonCase& testCase : buttonCases()) {
        auto button = testCase.create();
        button->setIconSize(QSize(18, 18));
        QCOMPARE(button->iconSize(), QSize(18, 18));

        button->setIconSize(QSize(28, 28));
        QCOMPARE(button->iconSize(), QSize(28, 28));
    }
}

void tst_ButtonIconSizeApi::setIconSizeKeepsSizeHintValid()
{
    for (const ButtonCase& testCase : buttonCases()) {
        auto button = testCase.create();
        button->setIconSize(QSize(18, 18));
        const QSize smallHint = button->sizeHint();

        button->setIconSize(QSize(28, 28));
        const QSize largeHint = button->sizeHint();

        QVERIFY2(smallHint.isValid(), qPrintable(testCase.name));
        QVERIFY2(largeHint.isValid(), qPrintable(testCase.name));
        QVERIFY2(largeHint.width() >= button->minimumSizeHint().width(), qPrintable(testCase.name));
        QVERIFY2(largeHint.height() >= button->minimumSizeHint().height(), qPrintable(testCase.name));
    }
}

void tst_ButtonIconSizeApi::setIconSizeKeepsRenderingValid()
{
    for (const ButtonCase& testCase : buttonCases()) {
        auto button = testCase.create();
        button->setIconSize(QSize(28, 28));
        button->resize(button->sizeHint().expandedTo(QSize(128, 56)));
        button->show();
        QTest::qWaitForWindowExposed(button.get());

        QPixmap pixmap(button->size() * 2);
        pixmap.setDevicePixelRatio(2.0);
        pixmap.fill(Qt::transparent);
        button->render(&pixmap);

        QVERIFY2(!pixmap.isNull(), qPrintable(testCase.name));
    }
}

void tst_ButtonIconSizeApi::repeatedSetIconSizeIsStable()
{
    for (const ButtonCase& testCase : buttonCases()) {
        auto button = testCase.create();
        button->setIconSize(QSize(24, 24));
        const QSize first = button->sizeHint();

        button->setIconSize(QSize(24, 24));
        const QSize second = button->sizeHint();

        QCOMPARE(first, second);
    }
}

QTEST_MAIN(tst_ButtonIconSizeApi)
#include "tst_button_icon_size_api.moc"
