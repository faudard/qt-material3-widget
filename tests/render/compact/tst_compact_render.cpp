#include <QtTest/QtTest>
class PlaceholderTest : public QObject {
    Q_OBJECT
private slots:
    void placeholder() { QVERIFY(true); }
};
QTEST_MAIN(PlaceholderTest)
#include "tst_compact_render.moc"
