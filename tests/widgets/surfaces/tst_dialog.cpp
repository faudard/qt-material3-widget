#include <QtTest/QtTest>

#include "qtmaterial/widgets/surfaces/qtmaterialdialog.h"
#include <QPushButton>

class tst_Dialog : public QObject
{
    Q_OBJECT

private slots:
    void escapeCloses();
    void openFocusesChild();
};

void tst_Dialog::escapeCloses()
{
    QWidget host;
    host.resize(800, 600);
    host.show();

    QtMaterial::QtMaterialDialog dialog(&host);
    dialog.resize(host.size());
    dialog.open();

    QVERIFY(dialog.isVisible());
    QTest::keyClick(&dialog, Qt::Key_Escape);
    QVERIFY(!dialog.isVisible());
}

void tst_Dialog::openFocusesChild()
{
    QWidget host;
    host.resize(800, 600);
    host.show();

    QtMaterial::QtMaterialDialog dialog(&host);
    auto* button = new QPushButton(QStringLiteral("Primary"), &dialog);
    button->setFocusPolicy(Qt::StrongFocus);
    button->move(40, 40);
    button->show();

    dialog.resize(host.size());
    dialog.open();

    QVERIFY(button->hasFocus() || dialog.hasFocus());
}

QTEST_MAIN(tst_Dialog)
#include "tst_dialog.moc"
