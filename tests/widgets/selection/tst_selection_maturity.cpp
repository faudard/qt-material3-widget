#include <QtTest/QtTest>

#include <QImage>
#include <QPainter>
#include <QSignalSpy>

#include "qtmaterial/widgets/selection/qtmaterialcheckbox.h"
#include "qtmaterial/widgets/selection/qtmaterialradiobutton.h"
#include "qtmaterial/widgets/selection/qtmaterialsegmentedbutton.h"
#include "qtmaterial/widgets/selection/qtmaterialswitch.h"

using namespace QtMaterial;

namespace {

QImage renderAtDpr2(QWidget& widget)
{
    const QSize logicalSize =
        widget.sizeHint().expandedTo(QSize(160, 48));
    widget.resize(logicalSize);

    QImage image(
        logicalSize.width() * 2,
        logicalSize.height() * 2,
        QImage::Format_ARGB32_Premultiplied);
    image.setDevicePixelRatio(2.0);
    image.fill(Qt::transparent);

    {
        QPainter painter(&image);
        widget.render(&painter);
    }

    return image;
}

} // namespace

class tst_SelectionMaturity : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void accessibleLabelsAndStateStaySynchronized();
    void disabledControlsIgnoreKeyboardActivation();
    void rtlDirectionalNavigationFollowsVisualDirection();
    void rtlSizeHintsRemainStable();
    void dpr2RenderingSmoke();
    void focusPoliciesAreProductionSafe();
    void checkboxTriStateContract();
    void segmentedSelectionContract();
};

void tst_SelectionMaturity::accessibleLabelsAndStateStaySynchronized()
{
    QtMaterialCheckbox checkbox;
    checkbox.setText(QStringLiteral("&Enable notifications"));
    QCOMPARE(
        checkbox.accessibleName(),
        QStringLiteral("Enable notifications"));

    QtMaterialRadioButton radio;
    radio.setText(QStringLiteral("Option &A"));
    QCOMPARE(radio.accessibleName(), QStringLiteral("Option A"));

    QtMaterialSwitch sw(QStringLiteral("Use dark mode"));
    QCOMPARE(sw.accessibleName(), QStringLiteral("Use dark mode"));
    QCOMPARE(sw.accessibleDescription(), QStringLiteral("Off"));
    sw.setChecked(true);
    QCOMPARE(sw.accessibleDescription(), QStringLiteral("On"));

    QtMaterialSegmentedButton segmented;
    QSignalSpy summarySpy(
        &segmented,
        &QtMaterialSegmentedButton::accessibilitySummaryChanged);

    segmented.addSegment(QStringLiteral("Day"));
    segmented.addSegment(QStringLiteral("Week"));
    segmented.addSegment(QStringLiteral("Month"));
    segmented.setCurrentIndex(1);

    QCOMPARE(segmented.accessibleName(), QStringLiteral("Segmented button"));
    QCOMPARE(
        segmented.accessibleDescription(),
        segmented.accessibilitySummary());
    QVERIFY(
        segmented.accessibleDescription().contains(
            QStringLiteral("Week")));
    QVERIFY(summarySpy.count() >= 4);
}

void tst_SelectionMaturity::disabledControlsIgnoreKeyboardActivation()
{
    QtMaterialCheckbox checkbox;
    checkbox.setEnabled(false);
    QTest::keyClick(&checkbox, Qt::Key_Space);
    QVERIFY(!checkbox.isChecked());

    QtMaterialRadioButton radio;
    radio.setEnabled(false);
    QTest::keyClick(&radio, Qt::Key_Space);
    QVERIFY(!radio.isChecked());

    QtMaterialSwitch sw;
    sw.setEnabled(false);
    QTest::keyClick(&sw, Qt::Key_Space);
    QVERIFY(!sw.isChecked());

    QtMaterialSegmentedButton segmented;
    segmented.addSegment(QStringLiteral("One"));
    segmented.addSegment(QStringLiteral("Two"));
    segmented.setSegmentEnabled(1, false);
    segmented.setCurrentIndex(0);

    QTest::keyClick(&segmented, Qt::Key_Right);
    QCOMPARE(segmented.currentIndex(), 0);
}

void tst_SelectionMaturity::rtlDirectionalNavigationFollowsVisualDirection()
{
    QtMaterialSwitch sw(QStringLiteral("Wi-Fi"));
    sw.setLayoutDirection(Qt::RightToLeft);
    sw.setChecked(false);

    QTest::keyClick(&sw, Qt::Key_Left);
    QVERIFY(sw.isChecked());

    QTest::keyClick(&sw, Qt::Key_Right);
    QVERIFY(!sw.isChecked());

    QtMaterialSegmentedButton segmented;
    segmented.addSegment(QStringLiteral("One"));
    segmented.addSegment(QStringLiteral("Two"));
    segmented.addSegment(QStringLiteral("Three"));
    segmented.setCurrentIndex(1);

    segmented.setLayoutDirection(Qt::LeftToRight);
    QTest::keyClick(&segmented, Qt::Key_Right);
    QCOMPARE(segmented.currentIndex(), 2);

    segmented.setCurrentIndex(1);
    segmented.setLayoutDirection(Qt::RightToLeft);
    QTest::keyClick(&segmented, Qt::Key_Right);
    QCOMPARE(segmented.currentIndex(), 0);
}

void tst_SelectionMaturity::rtlSizeHintsRemainStable()
{
    QtMaterialCheckbox checkbox;
    checkbox.setText(QStringLiteral("Checkbox"));
    const QSize checkboxLtr =
        static_cast<QWidget&>(checkbox).sizeHint();
    checkbox.setLayoutDirection(Qt::RightToLeft);
    QCOMPARE(
        static_cast<QWidget&>(checkbox).sizeHint(),
        checkboxLtr);

    QtMaterialRadioButton radio(QStringLiteral("Radio"));
    const QSize radioLtr = radio.sizeHint();
    radio.setLayoutDirection(Qt::RightToLeft);
    QCOMPARE(radio.sizeHint(), radioLtr);

    QtMaterialSwitch sw(QStringLiteral("Switch"));
    const QSize switchLtr = sw.sizeHint();
    sw.setLayoutDirection(Qt::RightToLeft);
    QCOMPARE(sw.sizeHint(), switchLtr);

    QtMaterialSegmentedButton segmented;
    segmented.addSegment(QStringLiteral("Day"));
    segmented.addSegment(QStringLiteral("Week"));
    const QSize segmentedLtr = segmented.sizeHint();
    segmented.setLayoutDirection(Qt::RightToLeft);
    QCOMPARE(segmented.sizeHint(), segmentedLtr);
}

void tst_SelectionMaturity::dpr2RenderingSmoke()
{
    QtMaterialCheckbox checkbox;
    checkbox.setText(QStringLiteral("Checkbox"));
    checkbox.setChecked(true);
    const QImage checkboxImage = renderAtDpr2(checkbox);
    QCOMPARE(checkboxImage.devicePixelRatio(), qreal(2.0));
    QVERIFY(!checkboxImage.isNull());

    QtMaterialRadioButton radio(QStringLiteral("Radio"));
    radio.setChecked(true);
    const QImage radioImage = renderAtDpr2(radio);
    QCOMPARE(radioImage.devicePixelRatio(), qreal(2.0));
    QVERIFY(!radioImage.isNull());

    QtMaterialSwitch sw(QStringLiteral("Switch"));
    sw.setChecked(true);
    const QImage switchImage = renderAtDpr2(sw);
    QCOMPARE(switchImage.devicePixelRatio(), qreal(2.0));
    QVERIFY(!switchImage.isNull());

    QtMaterialSegmentedButton segmented;
    segmented.addSegment(QStringLiteral("Day"));
    segmented.addSegment(QStringLiteral("Week"));
    segmented.setCurrentIndex(0);
    const QImage segmentedImage = renderAtDpr2(segmented);
    QCOMPARE(segmentedImage.devicePixelRatio(), qreal(2.0));
    QVERIFY(!segmentedImage.isNull());
}

void tst_SelectionMaturity::focusPoliciesAreProductionSafe()
{
    QtMaterialCheckbox checkbox;
    QtMaterialRadioButton radio;
    QtMaterialSwitch sw;
    QtMaterialSegmentedButton segmented;

    QCOMPARE(checkbox.focusPolicy(), Qt::StrongFocus);
    QCOMPARE(radio.focusPolicy(), Qt::StrongFocus);
    QCOMPARE(sw.focusPolicy(), Qt::StrongFocus);
    QVERIFY(segmented.focusPolicy() != Qt::NoFocus);
}

void tst_SelectionMaturity::checkboxTriStateContract()
{
    QtMaterialCheckbox checkbox;
    checkbox.setTristate(true);
    checkbox.setCheckState(Qt::PartiallyChecked);
    QCOMPARE(checkbox.checkState(), Qt::PartiallyChecked);

    QSignalSpy stateSpy(&checkbox, &QCheckBox::stateChanged);
    QTest::keyClick(&checkbox, Qt::Key_Space);
    QVERIFY(stateSpy.count() >= 1);
    QVERIFY(checkbox.checkState() != Qt::PartiallyChecked);
}

void tst_SelectionMaturity::segmentedSelectionContract()
{
    QtMaterialSegmentedButton segmented;
    segmented.addSegment(QStringLiteral("Day"));
    segmented.addSegment(QStringLiteral("Week"));
    segmented.addSegment(QStringLiteral("Month"));
    segmented.setCurrentIndex(1);

    QTest::keyClick(&segmented, Qt::Key_Home);
    QCOMPARE(segmented.currentIndex(), 0);
    QTest::keyClick(&segmented, Qt::Key_End);
    QCOMPARE(segmented.currentIndex(), 2);

    segmented.setSegmentEnabled(1, false);
    segmented.setCurrentIndex(0);
    QTest::keyClick(&segmented, Qt::Key_Right);
    QCOMPARE(segmented.currentIndex(), 2);
}

QTEST_MAIN(tst_SelectionMaturity)
#include "tst_selection_maturity.moc"
