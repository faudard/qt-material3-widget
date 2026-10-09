#include <QtTest/QtTest>

#include <QProgressBar>
#include <QSignalSpy>
#include <QStyle>
#include <QVBoxLayout>
#include <QWidget>

#include "qtmaterial/widgets/native/qtmaterialprogressbaradapter.h"

using namespace QtMaterial;

class tst_NativeProgressBarAdapter : public QObject
{
    Q_OBJECT

private slots:
    void preservesNativeContract();
    void supportsBusyAndOrientation();
    void adaptsTreeAndHonorsOptOut();
    void restoresStyle();
};

void tst_NativeProgressBarAdapter::preservesNativeContract()
{
    QProgressBar progress;
    progress.setRange(-20, 80);
    progress.setValue(30);
    progress.setFormat(QStringLiteral("%v / %m"));
    progress.setTextVisible(true);
    progress.setInvertedAppearance(true);

    QSignalSpy changed(
        &progress,
        &QProgressBar::valueChanged);

    QtMaterialProgressBarAdapter::apply(&progress);

    QVERIFY(
        QtMaterialProgressBarAdapter::isApplied(&progress));
    QCOMPARE(progress.minimum(), -20);
    QCOMPARE(progress.maximum(), 80);
    QCOMPARE(progress.value(), 30);
    QCOMPARE(progress.format(), QStringLiteral("%v / %m"));
    QVERIFY(progress.isTextVisible());
    QVERIFY(progress.invertedAppearance());

    progress.setValue(35);
    QCOMPARE(progress.value(), 35);
    QCOMPARE(changed.count(), 1);
}

void tst_NativeProgressBarAdapter::supportsBusyAndOrientation()
{
    QProgressBar progress;
    QtMaterialProgressBarAdapter::apply(&progress);

    progress.setRange(0, 0);
    progress.resize(240, 24);
    progress.show();
    QTest::qWait(60);
    QVERIFY(progress.isVisible());

    progress.setOrientation(Qt::Vertical);
    progress.setRange(0, 100);
    progress.setValue(60);
    progress.setInvertedAppearance(false);
    progress.resize(24, 240);
    progress.update();
    QCOMPARE(progress.orientation(), Qt::Vertical);
    QCOMPARE(progress.value(), 60);
}

void tst_NativeProgressBarAdapter::
    adaptsTreeAndHonorsOptOut()
{
    QWidget root;
    auto* layout = new QVBoxLayout(&root);
    auto* first = new QProgressBar(&root);
    auto* second = new QProgressBar(&root);
    layout->addWidget(first);
    layout->addWidget(second);

    QtMaterialProgressBarAdapter::setOptOut(
        second,
        true);

    QCOMPARE(
        QtMaterialProgressBarAdapter::applyToDescendants(
            &root),
        1);
    QVERIFY(
        QtMaterialProgressBarAdapter::isApplied(first));
    QVERIFY(
        !QtMaterialProgressBarAdapter::isApplied(second));
}

void tst_NativeProgressBarAdapter::restoresStyle()
{
    QProgressBar progress;
    QStyle* originalStyle = progress.style();

    QtMaterialProgressBarAdapter::apply(&progress);
    QVERIFY(progress.style() != originalStyle);

    QtMaterialProgressBarAdapter::remove(&progress);
    QVERIFY(
        !QtMaterialProgressBarAdapter::isApplied(&progress));
    QCOMPARE(progress.style(), originalStyle);
}

QTEST_MAIN(tst_NativeProgressBarAdapter)
#include "tst_nativeprogressbaradapter.moc"
