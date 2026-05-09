#include <QtTest/QtTest>

#include <QAbstractButton>
#include <QImage>
#include <QPixmap>
#include <QSignalSpy>
#include <QStyle>
#include <QtMath>

#include <memory>

#include "qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h"

namespace {

std::unique_ptr<QtMaterial::QtMaterialOutlinedButton> createButton()
{
    auto button = std::make_unique<QtMaterial::QtMaterialOutlinedButton>();
    button->setText(QStringLiteral("Outlined"));
    button->setAccessibleName(QStringLiteral("Outlined action"));
    button->setIcon(button->style()->standardIcon(QStyle::SP_DialogOpenButton));
    return button;
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

int paintedPixelCount(const QImage& image)
{
    int count = 0;
    for (int y = 0; y < image.height(); ++y) {
        const QRgb* line = reinterpret_cast<const QRgb*>(image.constScanLine(y));
        for (int x = 0; x < image.width(); ++x) {
            if (qAlpha(line[x]) != 0) {
                ++count;
            }
        }
    }
    return count;
}

bool differs(const QImage& lhs, const QImage& rhs)
{
    if (lhs.size() != rhs.size() || lhs.format() != rhs.format()) {
        return true;
    }

    for (int y = 0; y < lhs.height(); ++y) {
        const QRgb* left = reinterpret_cast<const QRgb*>(lhs.constScanLine(y));
        const QRgb* right = reinterpret_cast<const QRgb*>(rhs.constScanLine(y));
        for (int x = 0; x < lhs.width(); ++x) {
            if (left[x] != right[x]) {
                return true;
            }
        }
    }

    return false;
}

} // namespace

class tst_OutlinedButtonRendering : public QObject
{
    Q_OBJECT

private slots:
    void rendersEnabledAndDisabled();
    void disabledDoesNotActivate();
    void rendersWithKeyboardFocus();
    void rtlSizeHintIsStable();
    void rendersAtMultipleDevicePixelRatios_data();
    void rendersAtMultipleDevicePixelRatios();
};

void tst_OutlinedButtonRendering::rendersEnabledAndDisabled()
{
    auto enabledButton = createButton();
    const QImage enabledImage = renderButton(*enabledButton, 1.0);
    QVERIFY(hasPaintedPixels(enabledImage));

    auto disabledButton = createButton();
    disabledButton->setEnabled(false);
    const QImage disabledImage = renderButton(*disabledButton, 1.0);
    QVERIFY(hasPaintedPixels(disabledImage));
    QVERIFY(differs(enabledImage, disabledImage));
}

void tst_OutlinedButtonRendering::disabledDoesNotActivate()
{
    auto button = createButton();
    button->setEnabled(false);
    button->resize(button->sizeHint().expandedTo(QSize(144, 56)));
    button->show();
    QTest::qWaitForWindowExposed(button.get());

    QSignalSpy clickedSpy(button.get(), &QAbstractButton::clicked);
    QVERIFY(clickedSpy.isValid());

    QTest::keyClick(button.get(), Qt::Key_Space);
    QTest::keyClick(button.get(), Qt::Key_Return);
    QTest::mouseClick(button.get(), Qt::LeftButton);

    QCOMPARE(clickedSpy.count(), 0);
}

void tst_OutlinedButtonRendering::rendersWithKeyboardFocus()
{
    auto button = createButton();
    button->resize(button->sizeHint().expandedTo(QSize(144, 56)));
    button->show();
    QTest::qWaitForWindowExposed(button.get());

    button->setFocus(Qt::TabFocusReason);
    QVERIFY(button->hasFocus());

    const QImage focusedImage = renderButton(*button, 1.0);
    QVERIFY(hasPaintedPixels(focusedImage));
}

void tst_OutlinedButtonRendering::rtlSizeHintIsStable()
{
    auto button = createButton();

    button->setLayoutDirection(Qt::LeftToRight);
    const QSize ltr = button->sizeHint();

    button->setLayoutDirection(Qt::RightToLeft);
    const QSize rtl = button->sizeHint();

    QCOMPARE(ltr, rtl);
}

void tst_OutlinedButtonRendering::rendersAtMultipleDevicePixelRatios_data()
{
    QTest::addColumn<qreal>("dpr");

    QTest::newRow("1.0") << 1.0;
    QTest::newRow("1.5") << 1.5;
    QTest::newRow("2.0") << 2.0;
}

void tst_OutlinedButtonRendering::rendersAtMultipleDevicePixelRatios()
{
    QFETCH(qreal, dpr);

    auto button = createButton();
    const QImage image = renderButton(*button, dpr);

    QVERIFY(!image.isNull());
    QVERIFY(hasPaintedPixels(image));
    QVERIFY(paintedPixelCount(image) > 0);
    QVERIFY(image.width() >= qCeil(144 * dpr));
    QVERIFY(image.height() >= qCeil(56 * dpr));
}

QTEST_MAIN(tst_OutlinedButtonRendering)
#include "tst_outlinedbutton_rendering.moc"
