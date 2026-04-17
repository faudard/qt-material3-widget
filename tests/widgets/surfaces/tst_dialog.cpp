#include <QtTest/QtTest>

#include <QLabel>
#include <QSignalSpy>
#include <QWidget>

#include "qtmaterial/widgets/surfaces/qtmaterialdialog.h"

class tst_Dialog : public QObject
{
    Q_OBJECT

private slots:
    void basicConstruction();
    void openCloseAndBodyWidget();
};

void tst_Dialog::basicConstruction()
{
    QWidget host;
    host.resize(640, 480);

    QtMaterial::QtMaterialDialog widget(&host);
    QVERIFY(widget.parentWidget() == &host);
    QVERIFY(widget.bodyWidget() == nullptr);
    QVERIFY(widget.minimumSizeHint().width() > 0);
}

void tst_Dialog::openCloseAndBodyWidget()
{
    QWidget host;
    host.resize(800, 600);
    host.show();

    QtMaterial::QtMaterialDialog dialog(&host);
    auto* body = new QLabel(QStringLiteral("Dialog body content"));
    body->setMinimumSize(260, 120);
    dialog.setBodyWidget(body);

    dialog.open();
    QCoreApplication::processEvents();

    QVERIFY(dialog.isVisible());
    QVERIFY(dialog.bodyWidget() == body);
    QCOMPARE(dialog.parentWidget(), &host);
    QVERIFY(dialog.geometry().center().x() >= host.rect().center().x() - 2);
    QVERIFY(dialog.geometry().center().x() <= host.rect().center().x() + 2);

    dialog.close();
    QCoreApplication::processEvents();
    QVERIFY(!dialog.isVisible());
}

QTEST_MAIN(tst_Dialog)
#include "tst_dialog.moc"
