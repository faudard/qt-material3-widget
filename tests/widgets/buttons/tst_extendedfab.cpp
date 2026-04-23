#include <QSignalSpy>
#include <QTest>

#include "qtmaterial/widgets/buttons/qtmaterialextendedfab.h"

class tst_ExtendedFab : public QObject {
    Q_OBJECT

private slots:
    void constructs();
    void constructsTextOnly();
    void sizeHintExpandsWithText();
    void sizeHintUpdatesAfterChangingText();
    void keyboardActivation();
    void respectsTouchTargetSize();
};

void tst_ExtendedFab::constructs()
{
    QtMaterial::QtMaterialExtendedFab widget(QIcon(), QStringLiteral("Compose"));
    QCOMPARE(widget.text(), QStringLiteral("Compose"));
}

void tst_ExtendedFab::constructsTextOnly()
{
    QtMaterial::QtMaterialExtendedFab widget(QStringLiteral("Compose"));
    QCOMPARE(widget.text(), QStringLiteral("Compose"));
}

void tst_ExtendedFab::sizeHintExpandsWithText()
{
    QtMaterial::QtMaterialExtendedFab iconOnly;
    QtMaterial::QtMaterialExtendedFab shortLabel(QStringLiteral("Go"));
    QtMaterial::QtMaterialExtendedFab longLabel(QStringLiteral("Compose message"));

    QVERIFY(shortLabel.sizeHint().width() > iconOnly.sizeHint().width());
    QVERIFY(longLabel.sizeHint().width() > shortLabel.sizeHint().width());
}

void tst_ExtendedFab::sizeHintUpdatesAfterChangingText()
{
    QtMaterial::QtMaterialExtendedFab widget;
    const int initialWidth = widget.sizeHint().width();

    widget.setText(QStringLiteral("Compose message"));

    QVERIFY(widget.sizeHint().width() > initialWidth);
    QCOMPARE(widget.minimumSizeHint(), widget.sizeHint());
}

void tst_ExtendedFab::keyboardActivation()
{
    QtMaterial::QtMaterialExtendedFab widget(QIcon(), QStringLiteral("Compose"));
    widget.resize(widget.sizeHint());
    widget.show();
    QVERIFY(QTest::qWaitForWindowExposed(&widget));
    widget.setFocus();
    QVERIFY(widget.hasFocus());

    QSignalSpy clickedSpy(&widget, &QAbstractButton::clicked);
    QVERIFY(clickedSpy.isValid());

    QTest::keyClick(&widget, Qt::Key_Return);
    QCOMPARE(clickedSpy.count(), 1);
}

void tst_ExtendedFab::respectsTouchTargetSize()
{
    QtMaterial::QtMaterialExtendedFab widget(QIcon(), QStringLiteral("Compose"));
    QVERIFY(widget.sizeHint().height() >= 56);
    QVERIFY(widget.sizeHint().width() >= 80);
    QVERIFY(widget.minimumSizeHint().height() >= 56);
}

QTEST_MAIN(tst_ExtendedFab)

#include "tst_extendedfab.moc"
