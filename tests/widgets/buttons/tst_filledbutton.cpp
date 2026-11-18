#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QFontMetricsF>
#include <QtMath>
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"

class tst_Filledbutton : public QObject
{
    Q_OBJECT
private slots:
    void basicConstruction();
    void keyboardActivation();
    void preferredWidthIncludesPaintInset();
};

void tst_Filledbutton::basicConstruction()
{
    QtMaterial::QtMaterialFilledButton widget;
    widget.setText(QStringLiteral("Filled"));
    QVERIFY(widget.sizeHint().height() >= 40);
    QCOMPARE(widget.focusPolicy(), Qt::StrongFocus);
}

void tst_Filledbutton::keyboardActivation()
{
    QtMaterial::QtMaterialFilledButton widget;
    widget.setText(QStringLiteral("Filled"));
    widget.show();
    QVERIFY(QTest::qWaitForWindowExposed(&widget));
    widget.setFocus();

    QSignalSpy clickedSpy(&widget, &QAbstractButton::clicked);
    QVERIFY(clickedSpy.isValid());

    QTest::keyClick(&widget, Qt::Key_Space);
    QVERIFY(clickedSpy.count() >= 1);

    QTest::keyClick(&widget, Qt::Key_Return);
    QVERIFY(clickedSpy.count() >= 2);
}

void tst_Filledbutton::preferredWidthIncludesPaintInset()
{
    class InspectableButton : public QtMaterial::QtMaterialFilledButton {
    public:
        using QtMaterial::QtMaterialTextButton::currentButtonSpec;
    };
    InspectableButton button;
    button.setExpressive(true);
    for (const QString& label : {QStringLiteral("Month"),
                                 QStringLiteral("Week"),
                                 QStringLiteral("Primary action with a long label")}) {
        button.setText(label);
        button.ensurePolished();
        const int width = button.sizeHint().width();
        const auto& spec = button.currentButtonSpec();
        const QFont labelFont = spec.hasResolvedLabelFont ? spec.labelFont : button.font();
        const int budget = width - 2 - 2 * spec.horizontalPadding;
        const QFontMetricsF metrics(labelFont);
        QVERIFY(budget >= qCeil(metrics.horizontalAdvance(label)));
        QCOMPARE(metrics.elidedText(label, Qt::ElideRight, budget), label);
    }
}

QTEST_MAIN(tst_Filledbutton)
#include "tst_filledbutton.moc"
