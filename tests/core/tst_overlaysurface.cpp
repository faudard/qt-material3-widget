#include <QtTest/QtTest>

#include <QApplication>
#include <QCoreApplication>
#include <QPoint>
#include <QRect>
#include <QWidget>

#include "qtmaterial/core/qtmaterialoverlaysurface.h"

class tst_OverlaySurface : public QObject
{
    Q_OBJECT

private slots:
    void mapsGeometryIntoOverlayParentCoordinates();
    void topLevelOverlayUsesGlobalGeometry();
    void hostHideAndShowRestoresOverlayVisibilityIntent();
};

void tst_OverlaySurface::mapsGeometryIntoOverlayParentCoordinates()
{
    QWidget window;
    window.resize(400, 300);

    QWidget hostContainer(&window);
    hostContainer.setGeometry(40, 50, 220, 160);

    QWidget host(&hostContainer);
    host.setGeometry(10, 12, 80, 30);

    QtMaterial::QtMaterialOverlaySurface overlay(&window);
    overlay.setHostWidget(&host);

    window.show();
    hostContainer.show();
    host.show();
    overlay.show();
    QCoreApplication::processEvents();

    const QRect expectedInitial(
        window.mapFromGlobal(host.mapToGlobal(QPoint(0, 0))),
        host.size());

    QCOMPARE(overlay.geometry(), expectedInitial);

    host.move(30, 18);
    QCoreApplication::processEvents();

    const QRect expectedAfterMove(
        window.mapFromGlobal(host.mapToGlobal(QPoint(0, 0))),
        host.size());

    QCOMPARE(overlay.geometry(), expectedAfterMove);

    host.resize(100, 36);
    QCoreApplication::processEvents();

    const QRect expectedAfterResize(
        window.mapFromGlobal(host.mapToGlobal(QPoint(0, 0))),
        host.size());

    QCOMPARE(overlay.geometry(), expectedAfterResize);
}

void tst_OverlaySurface::topLevelOverlayUsesGlobalGeometry()
{
    QWidget window;
    window.resize(320, 240);

    QWidget host(&window);
    host.setGeometry(25, 35, 90, 28);

    QtMaterial::QtMaterialOverlaySurface overlay(nullptr);
    overlay.setHostWidget(&host);

    window.show();
    host.show();
    overlay.show();
    QCoreApplication::processEvents();

    const QRect expected(
        host.mapToGlobal(QPoint(0, 0)),
        host.size());

    QCOMPARE(overlay.geometry(), expected);
}

void tst_OverlaySurface::hostHideAndShowRestoresOverlayVisibilityIntent()
{
    QWidget window;
    window.resize(320, 240);

    QWidget host(&window);
    host.setGeometry(20, 20, 100, 40);

    QtMaterial::QtMaterialOverlaySurface overlay(&window);
    overlay.setHostWidget(&host);

    window.show();
    host.show();
    overlay.show();
    QCoreApplication::processEvents();

    QVERIFY(overlay.isVisible());

    host.hide();
    QCoreApplication::processEvents();

    QVERIFY(!overlay.isVisible());

    host.show();
    QCoreApplication::processEvents();

    QVERIFY(overlay.isVisible());

    overlay.hide();
    QCoreApplication::processEvents();

    host.hide();
    QCoreApplication::processEvents();
    QVERIFY(!overlay.isVisible());

    host.show();
    QCoreApplication::processEvents();

    QVERIFY(!overlay.isVisible());
}

QTEST_MAIN(tst_OverlaySurface)
#include "tst_overlaysurface.moc"