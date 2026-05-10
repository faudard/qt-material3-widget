#include <QtTest/QtTest>

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
    std::function<std::unique_ptr<QtMaterial::QtMaterialTextButton>()> create;
};

QVector<ButtonCase> cases()
{
    return {
        {
            QStringLiteral("text"),
            [] { return std::make_unique<QtMaterial::QtMaterialTextButton>(); },
        },
        {
            QStringLiteral("filled"),
            [] { return std::make_unique<QtMaterial::QtMaterialFilledButton>(); },
        },
        {
            QStringLiteral("filled-tonal"),
            [] { return std::make_unique<QtMaterial::QtMaterialFilledTonalButton>(); },
        },
        {
            QStringLiteral("outlined"),
            [] { return std::make_unique<QtMaterial::QtMaterialOutlinedButton>(); },
        },
        {
            QStringLiteral("elevated"),
            [] { return std::make_unique<QtMaterial::QtMaterialElevatedButton>(); },
        },
    };
}

} // namespace

class tst_ButtonTextLayout : public QObject
{
    Q_OBJECT

private slots:
    void iconAndTextSizeHintIsAtLeastTextOnly();
    void iconOnlySizeHintIsValidForTextBearingButtons();
    void rtlSizeHintMatchesLtr();
    void longTextIncreasesWidth();
};

void tst_ButtonTextLayout::iconAndTextSizeHintIsAtLeastTextOnly()
{
    for (const ButtonCase& testCase : cases()) {
        auto textOnly = testCase.create();
        textOnly->setText(QStringLiteral("Save"));
        const QSize textOnlyHint = textOnly->sizeHint();

        auto iconAndText = testCase.create();
        iconAndText->setText(QStringLiteral("Save"));
        iconAndText->setIcon(contractIcon());
        const QSize iconAndTextHint = iconAndText->sizeHint();

        QVERIFY2(iconAndTextHint.width() >= textOnlyHint.width(), qPrintable(testCase.name));
        QVERIFY2(iconAndTextHint.height() >= textOnlyHint.height(), qPrintable(testCase.name));
    }
}

void tst_ButtonTextLayout::iconOnlySizeHintIsValidForTextBearingButtons()
{
    for (const ButtonCase& testCase : cases()) {
        auto button = testCase.create();
        button->setIcon(contractIcon());
        const QSize hint = button->sizeHint();
        const QSize minimumHint = button->minimumSizeHint();

        QVERIFY2(hint.isValid(), qPrintable(testCase.name));
        QVERIFY2(minimumHint.isValid(), qPrintable(testCase.name));
        QVERIFY2(hint.width() >= minimumHint.width(), qPrintable(testCase.name));
        QVERIFY2(hint.height() >= minimumHint.height(), qPrintable(testCase.name));
    }
}

void tst_ButtonTextLayout::rtlSizeHintMatchesLtr()
{
    for (const ButtonCase& testCase : cases()) {
        auto button = testCase.create();
        button->setText(QStringLiteral("Save"));
        button->setIcon(contractIcon());

        button->setLayoutDirection(Qt::LeftToRight);
        const QSize ltr = button->sizeHint();

        button->setLayoutDirection(Qt::RightToLeft);
        const QSize rtl = button->sizeHint();

        QCOMPARE(ltr, rtl);
    }
}

void tst_ButtonTextLayout::longTextIncreasesWidth()
{
    for (const ButtonCase& testCase : cases()) {
        auto shortButton = testCase.create();
        shortButton->setText(QStringLiteral("Save"));
        const QSize shortHint = shortButton->sizeHint();

        auto longButton = testCase.create();
        longButton->setText(QStringLiteral("Create project from selected template"));
        const QSize longHint = longButton->sizeHint();

        QVERIFY2(longHint.width() > shortHint.width(), qPrintable(testCase.name));
        QCOMPARE(longHint.height(), shortHint.height());
    }
}

QTEST_MAIN(tst_ButtonTextLayout)
#include "tst_button_text_layout.moc"
