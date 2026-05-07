#include <QSignalSpy>
#include <QTest>
#include <QToolTip>

#include "qtmaterial/widgets/buttons/qtmaterialiconbutton.h"

class tst_QtMaterialIconButton : public QObject
{
    Q_OBJECT

private slots:
    void constructs();
    void keyboardActivationWithSpaceAndEnter();
    void selectionState();
    void checkableToggleMirrorsSelectedState();
    void sizeHintUsesTouchTarget();
    void explicitAccessibleNameIsAccepted();
    void tooltipProvidesAccessibleFallback();
    void emptyIconOnlyButtonCanBeDetected();
    void materialMetadataIsInitialized();
};

void tst_QtMaterialIconButton::constructs()
{
    QtMaterial::QtMaterialIconButton button;

    QVERIFY(button.isEnabled());
    QCOMPARE(button.focusPolicy(), Qt::StrongFocus);
    QVERIFY(button.requiresAccessibleName());
    QVERIFY(!button.hasUsableAccessibleName());
}

void tst_QtMaterialIconButton::keyboardActivationWithSpaceAndEnter()
{
    QtMaterial::QtMaterialIconButton button;
    button.setAccessibleName(QStringLiteral("Open"));
    button.setIcon(QIcon::fromTheme(QStringLiteral("document-open")));
    button.show();

    QVERIFY(QTest::qWaitForWindowExposed(&button));

    QSignalSpy clickedSpy(&button, &QAbstractButton::clicked);
    QVERIFY(clickedSpy.isValid());

    button.setFocus();
    QVERIFY(button.hasFocus());

    QTest::keyClick(&button, Qt::Key_Space);
    QCOMPARE(clickedSpy.count(), 1);

    QTest::keyClick(&button, Qt::Key_Return);
    QCOMPARE(clickedSpy.count(), 2);

    QTest::keyClick(&button, Qt::Key_Enter);
    QCOMPARE(clickedSpy.count(), 3);
}

void tst_QtMaterialIconButton::selectionState()
{
    QtMaterial::QtMaterialIconButton button;

    QVERIFY(!button.isSelected());
    button.setSelected(true);
    QVERIFY(button.isSelected());
    button.setSelected(false);
    QVERIFY(!button.isSelected());
}

void tst_QtMaterialIconButton::checkableToggleMirrorsSelectedState()
{
    QtMaterial::QtMaterialIconButton button;
    button.setAccessibleName(QStringLiteral("Favorite"));
    button.setCheckable(true);
    button.show();

    QVERIFY(QTest::qWaitForWindowExposed(&button));
    QVERIFY(!button.isChecked());
    QVERIFY(!button.isSelected());

    QTest::mouseClick(&button, Qt::LeftButton);
    QVERIFY(button.isChecked());
    QVERIFY(button.isSelected());

    QTest::mouseClick(&button, Qt::LeftButton);
    QVERIFY(!button.isChecked());
    QVERIFY(!button.isSelected());
}

void tst_QtMaterialIconButton::sizeHintUsesTouchTarget()
{
    QtMaterial::QtMaterialIconButton button;
    const QSize hint = button.sizeHint();

    QVERIFY(hint.width() >= 48);
    QVERIFY(hint.height() >= 48);
}

void tst_QtMaterialIconButton::explicitAccessibleNameIsAccepted()
{
    QtMaterial::QtMaterialIconButton button;
    button.setAccessibleName(QStringLiteral("Search"));

    QVERIFY(button.hasUsableAccessibleName());
    QCOMPARE(button.effectiveAccessibleName(), QStringLiteral("Search"));
}

void tst_QtMaterialIconButton::tooltipProvidesAccessibleFallback()
{
    QtMaterial::QtMaterialIconButton button;
    button.setToolTip(QStringLiteral("Refresh"));
    QCoreApplication::sendPostedEvents(&button, QEvent::ToolTipChange);
    QCoreApplication::processEvents();

    QVERIFY(button.hasUsableAccessibleName());
    QCOMPARE(button.effectiveAccessibleName(), QStringLiteral("Refresh"));
    QCOMPARE(button.accessibleName(), QStringLiteral("Refresh"));
}

void tst_QtMaterialIconButton::emptyIconOnlyButtonCanBeDetected()
{
    QtMaterial::QtMaterialIconButton button;
    button.setIcon(QIcon::fromTheme(QStringLiteral("edit-delete")));

    QVERIFY(button.requiresAccessibleName());
    QVERIFY(!button.hasUsableAccessibleName());

    button.setRequiresAccessibleName(false);
    QVERIFY(!button.requiresAccessibleName());
}

void tst_QtMaterialIconButton::materialMetadataIsInitialized()
{
    QtMaterial::QtMaterialIconButton button;

    QCOMPARE(button.materialComponent(), QStringLiteral("button"));
    QCOMPARE(button.materialVariant(), QStringLiteral("icon"));
    QCOMPARE(button.materialRole(), QStringLiteral("action"));
}

QTEST_MAIN(tst_QtMaterialIconButton)
#include "tst_iconbutton.moc"
