#include <QtTest/QtTest>
#include <type_traits>
#include "qtmaterial/foundation/qtmaterialqtcompat.h"

class QtCompatTest : public QObject
{
    Q_OBJECT
private slots:
    void compileTimeMajorContract();
    void helperSurfaceCompiles();
    void dprNullEventIsSafe();
};

void QtCompatTest::compileTimeMajorContract()
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    static_assert(QtMaterial::QtCompat::QtMajorVersion == 5, "Qt5 mismatch");
    static_assert(std::is_same<QtMaterial::QtCompat::EnterEvent, QEvent>::value,
                  "Qt5 enter event must be QEvent");
#else
    static_assert(QtMaterial::QtCompat::QtMajorVersion == 6, "Qt6 mismatch");
    static_assert(std::is_same<QtMaterial::QtCompat::EnterEvent, QEnterEvent>::value,
                  "Qt6 enter event must be QEnterEvent");
#endif
    QVERIFY(true);
}

void QtCompatTest::helperSurfaceCompiles()
{
    QVERIFY(&QtMaterial::QtCompat::mousePosition != nullptr);
    QVERIFY(&QtMaterial::QtCompat::mouseGlobalPosition != nullptr);
    QVERIFY(&QtMaterial::QtCompat::wheelPosition != nullptr);
}

void QtCompatTest::dprNullEventIsSafe()
{
    QVERIFY(!QtMaterial::QtCompat::isDevicePixelRatioChange(nullptr));
}

QTEST_MAIN(QtCompatTest)
#include "tst_qtcompat.moc"
