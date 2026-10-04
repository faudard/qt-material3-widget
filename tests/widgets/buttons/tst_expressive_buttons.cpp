#include <QtTest/QtTest>

#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/widgets/buttons/qtmaterialfab.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"

using namespace QtMaterial;

class ExpressiveButtonsTest : public QObject
{
    Q_OBJECT

private slots:
    void commonButtonSizeScale()
    {
        QtMaterialFilledButton button(QStringLiteral("Action"));
        QVERIFY(!button.expressive());

        button.setExpressive(true);
        QCOMPARE(button.expressiveShape(), QtMaterialButtonShape::Round);

        struct Case {
            QtMaterialButtonSize size;
            int expectedHeight;
        };

        const Case cases[] = {
            {QtMaterialButtonSize::ExtraSmall, 48},
            {QtMaterialButtonSize::Small, 48},
            {QtMaterialButtonSize::Medium, 56},
            {QtMaterialButtonSize::Large, 96},
            {QtMaterialButtonSize::ExtraLarge, 136},
        };

        for (const Case& item : cases) {
            button.setExpressiveSize(item.size);
            QCOMPARE(button.expressiveSize(), item.size);
            QCOMPARE(button.sizeHint().height(), item.expectedHeight);
        }
    }

    void shapeMorphTracksCheckedState()
    {
        QtMaterialFilledButton button(QStringLiteral("Toggle"));
        button.setExpressive(true);
        button.setExpressiveShape(QtMaterialButtonShape::Square);
        button.setCheckable(true);

        auto* transition =
            button.findChild<QtMaterialTransitionController*>(
                QStringLiteral("_qtm3_button_shape_transition"));
        QVERIFY(transition);

        button.setChecked(true);
        transition->finish();
        QCOMPARE(transition->progress(), qreal(1.0));

        button.setChecked(false);
        transition->finish();
        QCOMPARE(transition->progress(), qreal(0.0));
    }

    void fabUsesDedicatedExpressiveScale()
    {
        QtMaterialFab fab;
        fab.setExpressive(true);

        struct Case {
            QtMaterialFabSize size;
            int expectedHeight;
        };

        const Case cases[] = {
            {QtMaterialFabSize::Small, 48},
            {QtMaterialFabSize::Standard, 56},
            {QtMaterialFabSize::Medium, 80},
            {QtMaterialFabSize::Large, 96},
        };

        for (const Case& item : cases) {
            fab.setFabSize(item.size);
            QCOMPARE(fab.fabSize(), item.size);
            QCOMPARE(fab.sizeHint(), QSize(item.expectedHeight, item.expectedHeight));
        }
    }

    void expressiveIsOptIn()
    {
        QtMaterialFilledButton button(QStringLiteral("Stable"));
        const QSize baseline = button.sizeHint();

        button.setExpressiveSize(QtMaterialButtonSize::ExtraLarge);
        button.setExpressiveShape(QtMaterialButtonShape::Square);

        QCOMPARE(button.sizeHint(), baseline);
        QVERIFY(!button.expressive());
    }
};

QTEST_MAIN(ExpressiveButtonsTest)
#include "tst_expressive_buttons.moc"
