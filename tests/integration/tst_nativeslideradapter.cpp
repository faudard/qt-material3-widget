#include <QtTest/QtTest>

#include <QImage>
#include <QPainter>
#include <QSignalSpy>
#include <QSlider>
#include <QStyle>
#include <QStyleOptionSlider>
#include <QVBoxLayout>
#include <QWidget>

#include "qtmaterial/widgets/native/qtmaterialslideradapter.h"

using namespace QtMaterial;

namespace {

class ExposedSlider final : public QSlider
{
public:
    explicit ExposedSlider(
        Qt::Orientation orientation =
            Qt::Horizontal,
        QWidget* parent = nullptr)
        : QSlider(orientation, parent)
    {
    }

    using QSlider::initStyleOption;
};

QImage renderSlider(
    QWidget& widget,
    const QSize& size)
{
    widget.resize(size);
    widget.ensurePolished();

    QImage image(
        size,
        QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    widget.render(&painter);
    painter.end();
    return image;
}

} // namespace

class tst_NativeSliderAdapter : public QObject
{
    Q_OBJECT

private slots:
    void preservesNativeContract();
    void densityPropertyIsLive();
    void materialGeometryDrivesNativeSubControls();
    void horizontalVerticalAndRtlRender();
    void adaptsTreeAndHonorsOptOut();
};

void tst_NativeSliderAdapter::preservesNativeContract()
{
    ExposedSlider slider;
    QStyle* originalStyle = slider.style();

    slider.setRange(-10, 90);
    slider.setValue(25);
    slider.setSingleStep(5);
    slider.setPageStep(20);
    slider.setTickPosition(
        QSlider::TicksBelow);
    slider.setInvertedAppearance(true);

    QtMaterialSliderAdapter::apply(
        &slider,
        Density::Default);

    QVERIFY(
        QtMaterialSliderAdapter::isApplied(
            &slider));
    QVERIFY(slider.style() != originalStyle);

    QCOMPARE(slider.minimum(), -10);
    QCOMPARE(slider.maximum(), 90);
    QCOMPARE(slider.value(), 25);
    QCOMPARE(slider.singleStep(), 5);
    QCOMPARE(slider.pageStep(), 20);
    QCOMPARE(
        slider.tickPosition(),
        QSlider::TicksBelow);
    QVERIFY(slider.invertedAppearance());

    QSignalSpy valueChanged(
        &slider,
        &QSlider::valueChanged);
    QTest::keyClick(
        &slider,
        Qt::Key_Right);
    QVERIFY(valueChanged.count() >= 1);

    QtMaterialSliderAdapter::remove(
        &slider);
    QVERIFY(
        !QtMaterialSliderAdapter::isApplied(
            &slider));
    QCOMPARE(slider.style(), originalStyle);
}

void tst_NativeSliderAdapter::densityPropertyIsLive()
{
    ExposedSlider slider;
    QtMaterialSliderAdapter::apply(&slider);

    slider.setProperty(
        QtMaterialSliderAdapter::
            densityPropertyName(),
        QStringLiteral("compact"));
    QCOMPARE(
        int(QtMaterialSliderAdapter::density(
            &slider)),
        int(Density::Compact));

    QtMaterialSliderAdapter::setDensity(
        &slider,
        Density::Comfortable);
    QCOMPARE(
        int(QtMaterialSliderAdapter::density(
            &slider)),
        int(Density::Comfortable));
}

void tst_NativeSliderAdapter::
    materialGeometryDrivesNativeSubControls()
{
    ExposedSlider slider;
    slider.resize(240, 48);
    slider.setRange(0, 100);
    slider.setValue(50);
    QtMaterialSliderAdapter::apply(&slider);

    QStyleOptionSlider option;
    slider.initStyleOption(&option);

    const QRect handle =
        slider.style()->subControlRect(
            QStyle::CC_Slider,
            &option,
            QStyle::SC_SliderHandle,
            &slider);
    const QRect groove =
        slider.style()->subControlRect(
            QStyle::CC_Slider,
            &option,
            QStyle::SC_SliderGroove,
            &slider);

    QVERIFY(!handle.isEmpty());
    QVERIFY(!groove.isEmpty());
    QCOMPARE(handle.width(), 20);
    QCOMPARE(handle.height(), 20);
    QVERIFY(
        qAbs(
            handle.center().x()
            - groove.center().x())
        <= 2);

    const QStyle::SubControl hit =
        slider.style()->hitTestComplexControl(
            QStyle::CC_Slider,
            &option,
            handle.center(),
            &slider);
    QCOMPARE(
        int(hit),
        int(QStyle::SC_SliderHandle));
}

void tst_NativeSliderAdapter::
    horizontalVerticalAndRtlRender()
{
    ExposedSlider horizontal(Qt::Horizontal);
    horizontal.setRange(0, 100);
    horizontal.setValue(65);
    QtMaterialSliderAdapter::apply(
        &horizontal);
    QVERIFY(
        !renderSlider(
             horizontal,
             QSize(240, 48))
             .isNull());

    ExposedSlider vertical(Qt::Vertical);
    vertical.setRange(0, 100);
    vertical.setValue(35);
    QtMaterialSliderAdapter::apply(
        &vertical,
        Density::Compact);
    QVERIFY(
        !renderSlider(
             vertical,
             QSize(48, 240))
             .isNull());

    horizontal.setLayoutDirection(
        Qt::RightToLeft);
    horizontal.setInvertedAppearance(false);
    QVERIFY(
        !renderSlider(
             horizontal,
             QSize(240, 48))
             .isNull());
}

void tst_NativeSliderAdapter::
    adaptsTreeAndHonorsOptOut()
{
    QWidget root;
    auto* layout = new QVBoxLayout(&root);
    auto* first =
        new QSlider(Qt::Horizontal, &root);
    auto* second =
        new QSlider(Qt::Horizontal, &root);
    layout->addWidget(first);
    layout->addWidget(second);

    QtMaterialSliderAdapter::setOptOut(
        second,
        true);

    QCOMPARE(
        QtMaterialSliderAdapter::
            applyToDescendants(
                &root,
                Density::Default),
        1);
    QVERIFY(
        QtMaterialSliderAdapter::isApplied(
            first));
    QVERIFY(
        !QtMaterialSliderAdapter::isApplied(
            second));
}

QTEST_MAIN(tst_NativeSliderAdapter)
#include "tst_nativeslideradapter.moc"
