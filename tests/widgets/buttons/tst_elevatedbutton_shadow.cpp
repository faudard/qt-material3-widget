#include <QtTest/QtTest>

#include <QAbstractButton>
#include <QImage>
#include <QPixmap>
#include <QSignalSpy>

#include "qtmaterial/widgets/buttons/qtmaterialelevatedbutton.h"

namespace {

QImage renderButton(QAbstractButton& button, qreal devicePixelRatio)
{
    const QSize logicalSize = button.sizeHint().expandedTo(QSize(128, 56));
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

std::unique_ptr<QtMaterial::QtMaterialElevatedButton> createButton()
{
    auto button = std::make_unique<QtMaterial::QtMaterialElevatedButton>();
    button->setText(QStringLiteral("Elevated"));
    button->setAccessibleName(QStringLiteral("Elevated action"));
    return button;
}

} // namespace

class tst_ElevatedButtonShadow : public QObject
{
    Q_OBJECT

private slots:
    void rendersEnabledAndDisabled();
    void disabledDoesNotActivate();
    void rendersWithKeyboardFocus();
    void rendersAtMultipleDevicePixelRatios_data();
    void rendersAtMultipleDevicePixelRatios();
};

void tst_ElevatedButtonShadow::rendersEnabledAndDisabled()
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

void tst_ElevatedButtonShadow::disabledDoesNotActivate()
{
    auto button = createButton();
    button->setEnabled(false);
    button->resize(button->sizeHint().expandedTo(QSize(128, 56)));
    button->show();
    QTest::qWaitForWindowExposed(button.get());

    QSignalSpy clickedSpy(button.get(), &QAbstractButton::clicked);
    QVERIFY(clickedSpy.isValid());

    QTest::keyClick(button.get(), Qt::Key_Space);
    QTest::keyClick(button.get(), Qt::Key_Return);
    QTest::mouseClick(button.get(), Qt::LeftButton);

    QCOMPARE(clickedSpy.count(), 0);
}

void tst_ElevatedButtonShadow::rendersWithKeyboardFocus()
{
    auto button = createButton();
    button->resize(button->sizeHint().expandedTo(QSize(128, 56)));
    button->show();
    QTest::qWaitForWindowExposed(button.get());

    button->setFocus(Qt::TabFocusReason);
    QVERIFY(button->hasFocus());

    const QImage focusedImage = renderButton(*button, 1.0);
    QVERIFY(hasPaintedPixels(focusedImage));
}

void tst_ElevatedButtonShadow::rendersAtMultipleDevicePixelRatios_data()
{
    QTest::addColumn<qreal>("dpr");

    QTest::newRow("1.0") << 1.0;
    QTest::newRow("1.5") << 1.5;
    QTest::newRow("2.0") << 2.0;
}

void tst_ElevatedButtonShadow::rendersAtMultipleDevicePixelRatios()
{
    QFETCH(qreal, dpr);

    auto button = createButton();
    const QImage image = renderButton(*button, dpr);

    QVERIFY(!image.isNull());
    QVERIFY(hasPaintedPixels(image));
    QVERIFY(image.width() >= qCeil(128 * dpr));
    QVERIFY(image.height() >= qCeil(56 * dpr));
}

QTEST_MAIN(tst_ElevatedButtonShadow)
#include "tst_elevatedbutton_shadow.moc"
