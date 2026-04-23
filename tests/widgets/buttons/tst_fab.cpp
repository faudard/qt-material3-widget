#include <QSignalSpy>
#include <QTest>

#include "qtmaterial/widgets/buttons/qtmaterialfab.h"

class tst_Fab : public QObject {
    Q_OBJECT

private slots:
    void constructs();
    void constructsWithIcon();
    void accessibleNameCanDescribeIconOnlyAction();
    void tooltipCanDescribeIconOnlyAction();
    void defaultVariantIsPrimary();
    void canChangeVariantWithoutChangingSize();
    void sizeHintIsStableForTextAndIconChanges();
    void keyboardActivation();
    void respectsTouchTargetSize();
};

void tst_Fab::constructs()
{
    QtMaterial::QtMaterialFab widget;
    QVERIFY(widget.text().isEmpty());
    QVERIFY(!widget.isCheckable());
}

void tst_Fab::constructsWithIcon()
{
    QPixmap pixmap(24, 24);
    pixmap.fill(Qt::black);
    const QIcon icon(pixmap);

    QtMaterial::QtMaterialFab widget(icon);

    QVERIFY(!widget.icon().isNull());
    QVERIFY(widget.text().isEmpty());
    QVERIFY(!widget.isCheckable());
}

void tst_Fab::accessibleNameCanDescribeIconOnlyAction()
{
    QtMaterial::QtMaterialFab widget;

    widget.setAccessibleName(QStringLiteral("Create"));

    QCOMPARE(widget.accessibleName(), QStringLiteral("Create"));
}

void tst_Fab::tooltipCanDescribeIconOnlyAction()
{
    QtMaterial::QtMaterialFab widget;

    widget.setToolTip(QStringLiteral("Create"));

    QCOMPARE(widget.toolTip(), QStringLiteral("Create"));
}

void tst_Fab::defaultVariantIsPrimary()
{
    QtMaterial::QtMaterialFab widget;

    QCOMPARE(widget.fabVariant(), QtMaterial::QtMaterialFabVariant::Primary);
}

void tst_Fab::canChangeVariantWithoutChangingSize()
{
    QtMaterial::QtMaterialFab widget;
    const QSize initialSize = widget.sizeHint();

    widget.setFabVariant(QtMaterial::QtMaterialFabVariant::Secondary);
    QCOMPARE(widget.fabVariant(), QtMaterial::QtMaterialFabVariant::Secondary);
    QCOMPARE(widget.sizeHint(), initialSize);

    widget.setFabVariant(QtMaterial::QtMaterialFabVariant::Tertiary);
    QCOMPARE(widget.fabVariant(), QtMaterial::QtMaterialFabVariant::Tertiary);
    QCOMPARE(widget.sizeHint(), initialSize);

    widget.setFabVariant(QtMaterial::QtMaterialFabVariant::Surface);
    QCOMPARE(widget.fabVariant(), QtMaterial::QtMaterialFabVariant::Surface);
    QCOMPARE(widget.sizeHint(), initialSize);
}

void tst_Fab::sizeHintIsStableForTextAndIconChanges()
{
    QtMaterial::QtMaterialFab widget;
    const QSize initialSize = widget.sizeHint();

    widget.setText(QStringLiteral("Ignored label"));
    QCOMPARE(widget.sizeHint(), initialSize);

    QPixmap pixmap(24, 24);
    pixmap.fill(Qt::black);
    widget.setIcon(QIcon(pixmap));
    QCOMPARE(widget.sizeHint(), initialSize);
    QCOMPARE(widget.minimumSizeHint(), widget.sizeHint());
}

void tst_Fab::keyboardActivation()
{
    QtMaterial::QtMaterialFab widget;
    widget.setIcon(QIcon());
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

void tst_Fab::respectsTouchTargetSize()
{
    QtMaterial::QtMaterialFab widget;
    QVERIFY(widget.sizeHint().height() >= 56);
    QVERIFY(widget.sizeHint().width() >= 56);
    QVERIFY(widget.minimumSizeHint().height() >= 56);
    QVERIFY(widget.minimumSizeHint().width() >= 56);
}

QTEST_MAIN(tst_Fab)

#include "tst_fab.moc"
