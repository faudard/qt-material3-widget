#include <QtTest/QtTest>

#include "qtmaterial/widgets/selection/qtmaterialradiobutton.h"

class tst_RadioButtonRender : public QObject
{
    Q_OBJECT

private slots:
    void rendersDefault();
    void rendersChecked();
};

void tst_RadioButtonRender::rendersDefault()
{
    QtMaterial::QtMaterialRadioButton widget(QStringLiteral("Radio"));
    widget.resize(180, 48);
    widget.show();
    QVERIFY(QTest::qWaitForWindowExposed(&widget));
    const QImage image = widget.grab().toImage();
    QVERIFY(!image.isNull());
}

void tst_RadioButtonRender::rendersChecked()
{
    QtMaterial::QtMaterialRadioButton widget(QStringLiteral("Radio"));
    widget.setChecked(true);
    widget.resize(180, 48);
    widget.show();
    QVERIFY(QTest::qWaitForWindowExposed(&widget));
    const QImage image = widget.grab().toImage();
    QVERIFY(!image.isNull());
}

QTEST_MAIN(tst_RadioButtonRender)
#include "tst_radiobutton_render.moc"
