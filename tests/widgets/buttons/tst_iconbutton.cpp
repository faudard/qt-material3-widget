#include <QtTest/QtTest>

#include "qtmaterial/widgets/buttons/qtmaterialiconbutton.h"

class tst_QtMaterialIconButton : public QObject
{
    Q_OBJECT
private slots:
    void constructs();
    void keyboardActivation();
    void selectionState();
    void sizeHintUsesTouchTarget();
};

void tst_QtMaterialIconButton::constructs()
{
    QtMaterial::QtMaterialIconButton button;
    QVERIFY(button.isEnabled());
}

void tst_QtMaterialIconButton::keyboardActivation()
{
    QtMaterial::QtMaterialIconButton button;
    button.setIcon(QIcon::fromTheme(QStringLiteral("document-open")));
    button.show();
    QVERIFY(QTest::qWaitForWindowExposed(&button));

    QSignalSpy clickedSpy(&button, &QAbstractButton::clicked);
    QVERIFY(clickedSpy.isValid());

    button.setFocus();
    QVERIFY(button.hasFocus());

    QTest::keyClick(&button, Qt::Key_Space);
    QCOMPARE(clickedSpy.count(), 1);
}

void tst_QtMaterialIconButton::selectionState()
{
    QtMaterial::QtMaterialIconButton button;
    QVERIFY(!button.isSelected());
    button.setSelected(true);
    QVERIFY(button.isSelected());
}

void tst_QtMaterialIconButton::sizeHintUsesTouchTarget()
{
    QtMaterial::QtMaterialIconButton button;
    const QSize hint = button.sizeHint();
    QVERIFY(hint.width() >= 48);
    QVERIFY(hint.height() >= 48);
}

QTEST_MAIN(tst_QtMaterialIconButton)
#include "tst_iconbutton.moc"
