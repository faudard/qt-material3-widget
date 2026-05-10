#include <QtTest/QtTest>

#include <QFontMetrics>
#include <QIcon>
#include <QPainter>
#include <QPixmap>

#include "widgets/buttons/private/qtmaterialbuttonrenderhelper_p.h"
#include "qtmaterial/specs/qtmaterialbuttonspec.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
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

QtMaterial::ButtonSpec layoutSpec()
{
    QtMaterial::ButtonSpec spec;
    spec.touchTarget = QSize(40, 40);
    spec.containerHeight = 40;
    spec.horizontalPadding = 8;
    spec.iconSize = 24;
    spec.iconSpacing = 8;
    return spec;
}

} // namespace

class tst_ButtonRenderHelperLayout : public QObject
{
    Q_OBJECT

private slots:
    void textRectUsesActualElidedTextWidth();
    void rtlMirrorsIconAndTextOrder();
    void ltrKeepsIconBeforeText();
    void publicSizeHintDoesNotExplodeForShortText();
    void publicSizeHintIsStableBetweenLtrAndRtl();
};

void tst_ButtonRenderHelperLayout::textRectUsesActualElidedTextWidth()
{
    QtMaterial::QtMaterialFilledButton button;
    button.setText(QStringLiteral("Save"));
    button.setIcon(contractIcon());

    const QtMaterial::ButtonSpec spec = layoutSpec();
    const QRect container(0, 0, 4000, 40);
    const QFont font = button.font();
    const auto layout = QtMaterial::ButtonRenderHelper::layoutContent(
        &button,
        spec,
        container,
        font,
        button.text());

    const int expectedWidth = QFontMetrics(font).horizontalAdvance(layout.elidedText);
    QCOMPARE(layout.textRect.width(), expectedWidth);
    QVERIFY(layout.textRect.width() < 4000);
}

void tst_ButtonRenderHelperLayout::rtlMirrorsIconAndTextOrder()
{
    QtMaterial::QtMaterialFilledButton button;
    button.setText(QStringLiteral("Save"));
    button.setIcon(contractIcon());
    button.setLayoutDirection(Qt::RightToLeft);

    const auto layout = QtMaterial::ButtonRenderHelper::layoutContent(
        &button,
        layoutSpec(),
        QRect(0, 0, 240, 40),
        button.font(),
        button.text());

    QVERIFY(layout.hasIcon);
    QVERIFY(layout.textRect.left() < layout.iconRect.left());
}

void tst_ButtonRenderHelperLayout::ltrKeepsIconBeforeText()
{
    QtMaterial::QtMaterialFilledButton button;
    button.setText(QStringLiteral("Save"));
    button.setIcon(contractIcon());
    button.setLayoutDirection(Qt::LeftToRight);

    const auto layout = QtMaterial::ButtonRenderHelper::layoutContent(
        &button,
        layoutSpec(),
        QRect(0, 0, 240, 40),
        button.font(),
        button.text());

    QVERIFY(layout.hasIcon);
    QVERIFY(layout.iconRect.left() < layout.textRect.left());
}

void tst_ButtonRenderHelperLayout::publicSizeHintDoesNotExplodeForShortText()
{
    QtMaterial::QtMaterialTextButton button;
    button.setText(QStringLiteral("Save"));
    button.setIcon(contractIcon());

    const QSize hint = button.sizeHint();
    QVERIFY(hint.isValid());
    QVERIFY(hint.width() >= 88);
    QVERIFY(hint.width() < 300);
}

void tst_ButtonRenderHelperLayout::publicSizeHintIsStableBetweenLtrAndRtl()
{
    QtMaterial::QtMaterialTextButton button;
    button.setText(QStringLiteral("Save"));
    button.setIcon(contractIcon());

    button.setLayoutDirection(Qt::LeftToRight);
    const QSize ltr = button.sizeHint();

    button.setLayoutDirection(Qt::RightToLeft);
    const QSize rtl = button.sizeHint();

    QCOMPARE(ltr, rtl);
}

QTEST_MAIN(tst_ButtonRenderHelperLayout)
#include "tst_button_render_helper_layout.moc"
