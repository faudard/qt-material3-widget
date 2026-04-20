#include <QtTest/QtTest>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QWidget>
#include "qtmaterial/widgets/surfaces/qtmaterialdialog.h"

class tst_Dialog : public QObject
{
    Q_OBJECT
private slots:
    void basicConstruction();
    void escapeClosesDialog();
};

void tst_Dialog::basicConstruction()
{
    QWidget host;
    host.resize(640, 480);
    QtMaterial::QtMaterialDialog widget(&host);
    QVERIFY(widget.parentWidget() == &host);
}

void tst_Dialog::escapeClosesDialog()
{
    QWidget host;
    host.resize(640, 480);
    host.show();
    QVERIFY(QTest::qWaitForWindowExposed(&host));

    QtMaterial::QtMaterialDialog dialog(&host);
    auto* editor = new QLineEdit;
    editor->setAccessibleName(QStringLiteral("Dialog editor"));
    dialog.setBodyWidget(editor);
    dialog.open();
    QVERIFY(dialog.isVisible());

    QTest::keyClick(&dialog, Qt::Key_Escape);
    QVERIFY(!dialog.isVisible());
}

QTEST_MAIN(tst_Dialog)
#include "tst_dialog.moc"
