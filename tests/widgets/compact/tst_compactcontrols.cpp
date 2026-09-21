#include <QtTest>

#include <type_traits>

#include "qtmaterial/widgets/selection/qtmaterialchip.h"
#include "qtmaterial/widgets/compact/qtmaterialfilterchip.h"
#include "qtmaterial/widgets/compact/qtmaterialinputchip.h"
#include "qtmaterial/widgets/selection/qtmaterialsegmentedbutton.h"

using namespace QtMaterial;

class tst_CompactControls : public QObject
{
    Q_OBJECT

private slots:
    void canonicalHeadersExposeTypes();
    void convenienceChipsConfigureVariants();
    void densityChangesResolvedMetrics();
    void inputChipRequestsRemoval();
};

void tst_CompactControls::canonicalHeadersExposeTypes()
{
    static_assert(
        std::is_base_of<
            QtMaterialAbstractButton,
            QtMaterialChip>::value,
        "Canonical chip header must expose QtMaterialChip.");

    static_assert(
        std::is_base_of<
            QtMaterialControl,
            QtMaterialSegmentedButton>::value,
        "Canonical segmented button header must expose QtMaterialSegmentedButton.");

    QtMaterialChip chip;
    QtMaterialSegmentedButton segmented;

    QCOMPARE(
        chip.variant(),
        ChipVariant::Assist);
    QCOMPARE(
        segmented.count(),
        0);
}

void tst_CompactControls::convenienceChipsConfigureVariants()
{
    QtMaterialFilterChip filter(
        QStringLiteral("Filter"));

    QCOMPARE(
        filter.variant(),
        ChipVariant::Filter);
    QVERIFY(filter.isCheckable());

    QtMaterialInputChip input(
        QStringLiteral("Input"));

    QCOMPARE(
        input.variant(),
        ChipVariant::Input);
    QVERIFY(input.isRemovable());
}

void tst_CompactControls::densityChangesResolvedMetrics()
{
    QtMaterialChip chip(
        QStringLiteral("Density"));

    chip.setDensity(Density::Compact);
    QCOMPARE(
        chip.resolvedSpec().containerHeight,
        28);

    chip.setDensity(Density::Comfortable);
    QCOMPARE(
        chip.resolvedSpec().containerHeight,
        36);
}

void tst_CompactControls::inputChipRequestsRemoval()
{
    QtMaterialInputChip input(
        QStringLiteral("Remove"));

    input.resize(input.sizeHint());

    QSignalSpy spy(
        &input,
        &QtMaterialChip::removeRequested);

    const ChipSpec spec =
        input.resolvedSpec();

    const QPoint removePoint(
        input.width()
            - spec.horizontalPadding
            - spec.iconSize / 2,
        input.height() / 2);

    QTest::mouseClick(
        &input,
        Qt::LeftButton,
        Qt::NoModifier,
        removePoint);

    QCOMPARE(spy.count(), 1);
}

QTEST_MAIN(tst_CompactControls)
#include "tst_compactcontrols.moc"
