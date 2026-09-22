#include <QtTest/QtTest>
#include <type_traits>

#include "qtmaterial/core/qtmaterialeventcompat.h"

class QtCompatTest : public QObject
{
    Q_OBJECT

private slots:
    void enterEventTypeMatchesQtMajor();
    void mousePositionHelperCompiles();
};

void QtCompatTest::enterEventTypeMatchesQtMajor()
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    static_assert(std::is_same<QtMaterial::EnterEvent, QEvent>::value,
                  "Qt5 enter event must be QEvent");
#else
    static_assert(std::is_same<QtMaterial::EnterEvent, QEnterEvent>::value,
                  "Qt6 enter event must be QEnterEvent");
#endif
    QVERIFY(true);
}

void QtCompatTest::mousePositionHelperCompiles()
{
    QVERIFY(&QtMaterial::mousePosition != nullptr);
}

QTEST_MAIN(QtCompatTest)
#include "tst_qtcompat.moc"
