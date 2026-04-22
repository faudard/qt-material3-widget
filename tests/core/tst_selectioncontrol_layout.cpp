#include <QtTest/QtTest>

#include <QFontMetrics>
#include <QPaintEvent>
#include <QRect>
#include <QSize>

#include "qtmaterial/core/qtmaterialselectioncontrol.h"

namespace {

class TestSelectionControl final : public QtMaterial::QtMaterialSelectionControl
{
public:
    explicit TestSelectionControl(QWidget* parent = nullptr)
        : QtMaterialSelectionControl(parent)
    {
    }

    using QtMaterialSelectionControl::indicatorRect;
    using QtMaterialSelectionControl::labelRect;

    QSize exposedSizeHint() const
    {
        return QtMaterialSelectionControl::sizeHint();
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
    }
};

} // namespace

class tst_SelectionControlLayout : public QObject
{
    Q_OBJECT

private slots:
    void defaultIsCheckable();
    void ltrGeometryPlacesIndicatorOnLeadingEdge();
    void rtlGeometryMirrorsIndicatorAndLabel();
    void spacingIsClampedToZero();
    void sizeHintUsesBoundedSpacingAndMinimumHeight();
};

void tst_SelectionControlLayout::defaultIsCheckable()
{
    TestSelectionControl control;
    QVERIFY(control.isCheckable());
}

void tst_SelectionControlLayout::ltrGeometryPlacesIndicatorOnLeadingEdge()
{
    TestSelectionControl control;
    control.resize(140, 40);
    control.setSpacing(12);
    control.setLayoutDirection(Qt::LeftToRight);

    QCOMPARE(control.indicatorRect(), QRect(0, 11, 18, 18));
    QCOMPARE(control.labelRect(), QRect(30, 0, 110, 40));
}

void tst_SelectionControlLayout::rtlGeometryMirrorsIndicatorAndLabel()
{
    TestSelectionControl control;
    control.resize(140, 40);
    control.setSpacing(12);
    control.setLayoutDirection(Qt::RightToLeft);

    QCOMPARE(control.indicatorRect(), QRect(122, 11, 18, 18));
    QCOMPARE(control.labelRect(), QRect(0, 0, 110, 40));
}

void tst_SelectionControlLayout::spacingIsClampedToZero()
{
    TestSelectionControl control;
    control.resize(140, 40);
    control.setSpacing(-8);

    QCOMPARE(control.spacing(), 0);
    QCOMPARE(control.indicatorRect(), QRect(0, 11, 18, 18));
    QCOMPARE(control.labelRect(), QRect(18, 0, 122, 40));
}

void tst_SelectionControlLayout::sizeHintUsesBoundedSpacingAndMinimumHeight()
{
    TestSelectionControl control;
    control.setText(QStringLiteral("Option"));
    control.setSpacing(-20);

    const QFontMetrics fm(control.font());
    const QSize hint = control.exposedSizeHint();

    QCOMPARE(control.spacing(), 0);
    QCOMPARE(hint.width(), 18 + fm.horizontalAdvance(QStringLiteral("Option")) + 16);
    QCOMPARE(hint.height(), qMax(48, fm.height() + 16));
}

QTEST_MAIN(tst_SelectionControlLayout)
#include "tst_selectioncontrol_layout.moc"