#include <QtTest/QtTest>

#include <QAbstractButton>
#include <QIcon>
#include <QPainter>
#include <QPixmap>
#include <memory>

#include "qtmaterial/widgets/buttons/qtmaterialelevatedbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledtonalbutton.h"
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

QVector<ButtonCase> textBearingCases()
{
    return {
        {QStringLiteral("text"), [] {
             auto button = std::make_unique<QtMaterial::QtMaterialTextButton>();
             button->setText(QStringLiteral("Save"));
             return button;
         }},
        {QStringLiteral("filled"), [] {
             auto button = std::make_unique<QtMaterial::QtMaterialFilledButton>();
             button->setText(QStringLiteral("Save"));
             return button;
         }},
        {QStringLiteral("filled-tonal"), [] {
             return std::make_unique<QtMaterial::QtMaterialFilledTonalButton>(QStringLiteral("Save"));
         }},
        {QStringLiteral("outlined"), [] {
             auto button = std::make_unique<QtMaterial::QtMaterialOutlinedButton>();
             button->setText(QStringLiteral("Save"));
             return button;
         }},
        {QStringLiteral("elevated"), [] {
             auto button = std::make_unique<QtMaterial::QtMaterialElevatedButton>();
             button->setText(QStringLiteral("Save"));
             return button;
         }},
    };
}

QImage renderButton(QAbstractButton& button, qreal devicePixelRatio)
{
    const QSize logicalSize = button.sizeHint().expandedTo(QSize(144, 56));
    button.resize(logicalSize);
    button.show();
    QTest::qWaitForWindowExposed(&button);

    QPixmap pixmap(qCeil(logicalSize.width() * devicePixelRatio),
                   qCeil(logicalSize.height() * devicePixelRatio));
    pixmap.setDevicePixelRatio(devicePixelRatio);
    pixmap.fill(Qt::transparent);

    button.render(&pixmap);
    return pixmap.toImage();
}

bool hasPaintedPixels(const QImage& image)
{
    if (image.isNull()) {
        return false;
    }

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

class tst_ButtonRenderHelperLayout : public QObject
{
    Q_OBJECT

private slots:
    void iconAndTextSizeHintIsAtLeastTextOnly();
    void publicSizeHintDoesNotExplodeForShortText();
    void publicSizeHintIsStableBetweenLtrAndRtl();
    void longTextIncreasesWidth();
    void rendersInLtrAndRtl();
};

void tst_ButtonRenderHelperLayout::iconAndTextSizeHintIsAtLeastTextOnly()
{
    for (const ButtonCase& testCase : textBearingCases()) {
        auto textOnly = testCase.create();
        const QSize textOnlyHint = textOnly->sizeHint();

        auto iconAndText = testCase.create();
        iconAndText->setIcon(contractIcon());
        const QSize iconAndTextHint = iconAndText->sizeHint();

        QVERIFY2(iconAndTextHint.width() >= textOnlyHint.width(), qPrintable(testCase.name));
        QVERIFY2(iconAndTextHint.height() >= textOnlyHint.height(), qPrintable(testCase.name));
    }
}

void tst_ButtonRenderHelperLayout::publicSizeHintDoesNotExplodeForShortText()
{
    for (const ButtonCase& testCase : textBearingCases()) {
        auto button = testCase.create();
        button->setIcon(contractIcon());
        const QSize hint = button->sizeHint();

        QVERIFY2(hint.isValid(), qPrintable(testCase.name));
        QVERIFY2(hint.width() >= 40, qPrintable(testCase.name));
        QVERIFY2(hint.height() >= 40, qPrintable(testCase.name));
        QVERIFY2(hint.width() < 300, qPrintable(testCase.name));
    }
}

void tst_ButtonRenderHelperLayout::publicSizeHintIsStableBetweenLtrAndRtl()
{
    for (const ButtonCase& testCase : textBearingCases()) {
        auto button = testCase.create();
        button->setIcon(contractIcon());

        button->setLayoutDirection(Qt::LeftToRight);
        const QSize ltr = button->sizeHint();

        button->setLayoutDirection(Qt::RightToLeft);
        const QSize rtl = button->sizeHint();

        QCOMPARE(ltr, rtl);
    }
}

void tst_ButtonRenderHelperLayout::longTextIncreasesWidth()
{
    for (const ButtonCase& testCase : textBearingCases()) {
        auto shortButton = testCase.create();
        const QSize shortHint = shortButton->sizeHint();

        auto longButton = testCase.create();
        longButton->setText(QStringLiteral("Create project from selected template"));
        const QSize longHint = longButton->sizeHint();

        QVERIFY2(longHint.width() > shortHint.width(), qPrintable(testCase.name));
        QCOMPARE(longHint.height(), shortHint.height());
    }
}

void tst_ButtonRenderHelperLayout::rendersInLtrAndRtl()
{
    for (const ButtonCase& testCase : textBearingCases()) {
        auto ltrButton = testCase.create();
        ltrButton->setIcon(contractIcon());
        ltrButton->setLayoutDirection(Qt::LeftToRight);
        QVERIFY2(hasPaintedPixels(renderButton(*ltrButton, 1.0)), qPrintable(testCase.name));

        auto rtlButton = testCase.create();
        rtlButton->setIcon(contractIcon());
        rtlButton->setLayoutDirection(Qt::RightToLeft);
        QVERIFY2(hasPaintedPixels(renderButton(*rtlButton, 1.0)), qPrintable(testCase.name));
    }
}

QTEST_MAIN(tst_ButtonRenderHelperLayout)
#include "tst_button_render_helper_layout.moc"
