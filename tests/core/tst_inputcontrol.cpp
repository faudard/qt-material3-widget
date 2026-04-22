#include <QtTest/QtTest>

#include <QRect>
#include <QSize>

#include "qtmaterial/core/qtmaterialinputcontrol.h"

namespace {

class TestInputControl final : public QtMaterial::QtMaterialInputControl
{
public:
    explicit TestInputControl(QWidget* parent = nullptr)
        : QtMaterialInputControl(parent)
    {
    }

    QRect exposedContentRect() const
    {
        return contentRect();
    }

    QRect exposedSupportingTextRect() const
    {
        return supportingTextRect();
    }

    bool errorFlag() const
    {
        return interactionState().hasError();
    }
};

} // namespace

class tst_InputControl : public QObject
{
    Q_OBJECT

private slots:
    void defaultTextsAndErrorState();
    void defaultRectsMatchCurrentContract();
    void smallGeometryIsClamped();
    void errorFlagMirrorsInteractionState();
};

void tst_InputControl::defaultTextsAndErrorState()
{
    TestInputControl control;

    QCOMPARE(control.labelText(), QString());
    QCOMPARE(control.supportingText(), QString());
    QCOMPARE(control.errorText(), QString());
    QVERIFY(!control.hasErrorState());
    QVERIFY(!control.errorFlag());
}

void tst_InputControl::defaultRectsMatchCurrentContract()
{
    TestInputControl control;
    control.resize(200, 60);

    QCOMPARE(control.exposedSupportingTextRect(), QRect(16, 40, 168, 16));
    QCOMPARE(control.exposedContentRect(), QRect(16, 8, 168, 28));
}

void tst_InputControl::smallGeometryIsClamped()
{
    TestInputControl control;
    control.resize(20, 20);

    const QRect supporting = control.exposedSupportingTextRect();
    const QRect content = control.exposedContentRect();

    QVERIFY(supporting.width() >= 0);
    QVERIFY(supporting.height() >= 0);
    QVERIFY(content.width() >= 0);
    QVERIFY(content.height() >= 0);

    QCOMPARE(supporting, QRect(16, 8, 0, 16));
    QCOMPARE(content, QRect(16, 8, 0, 0));
}

void tst_InputControl::errorFlagMirrorsInteractionState()
{
    TestInputControl control;

    control.setHasErrorState(true);
    QVERIFY(control.hasErrorState());
    QVERIFY(control.errorFlag());

    control.setHasErrorState(false);
    QVERIFY(!control.hasErrorState());
    QVERIFY(!control.errorFlag());
}

QTEST_MAIN(tst_InputControl)
#include "tst_inputcontrol.moc"