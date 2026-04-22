#include <QtTest>
#include <QCoreApplication>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QSize>

#include "qtmaterial/core/qtmaterialabstractbutton.h"
#include "qtmaterial/core/qtmaterialinteractionstate.h"

namespace {

class TestAbstractButton final : public QtMaterial::QtMaterialAbstractButton
{
public:
    using QtMaterialAbstractButton::setCheckable;
    using QtMaterialAbstractButton::setChecked;
    using QtMaterialAbstractButton::setDown;

    explicit TestAbstractButton(QWidget* parent = nullptr)
        : QtMaterialAbstractButton(parent)
    {
        resize(120, 40);
    }

    const QtMaterial::QtMaterialInteractionState& state() const noexcept
    {
        return interactionState();
    }

    int stateChangedCount() const noexcept
    {
        return m_stateChangedCount;
    }

    void resetStateChangedCount() noexcept
    {
        m_stateChangedCount = 0;
    }

    QSize sizeHint() const override
    {
        return QSize(120, 40);
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
    }

    void stateChangedEvent() override
    {
        ++m_stateChangedCount;
        QtMaterialAbstractButton::stateChangedEvent();
    }

private:
    int m_stateChangedCount = 0;
};

static QMouseEvent makeMouseEvent(QEvent::Type type,
                                  const QPoint& pos,
                                  Qt::MouseButton button,
                                  Qt::MouseButtons buttons)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return QMouseEvent(type,
                       QPointF(pos),
                       QPointF(pos),
                       QPointF(pos),
                       button,
                       buttons,
                       Qt::NoModifier);
#else
    return QMouseEvent(type,
                       QPointF(pos),
                       button,
                       buttons,
                       Qt::NoModifier);
#endif
}

} // namespace

class tst_AbstractButtonState : public QObject
{
    Q_OBJECT

private slots:
    void defaultsMirrorBaseState();
    void setDownSynchronizesPressed();
    void checkedRequiresCheckable();
    void leftMousePressAndReleaseSynchronizePressed();
    void rightMousePressDoesNotSetPressed();
    void disabledButtonDoesNotEnterPressedState();
    void focusEventsSynchronizeFocused();
    void returnKeyTogglesCheckedWhenCheckable();
};

void tst_AbstractButtonState::defaultsMirrorBaseState()
{
    TestAbstractButton button;

    QVERIFY(button.state().isEnabled());
    QVERIFY(!button.state().isPressed());
    QVERIFY(!button.state().isChecked());
    QVERIFY(!button.state().isCheckable());
    QVERIFY(!button.state().isHovered());
    QVERIFY(!button.state().isFocused());
}

void tst_AbstractButtonState::setDownSynchronizesPressed()
{
    TestAbstractButton button;

    button.setDown(true);
    QVERIFY(button.isDown());
    QVERIFY(button.state().isPressed());

    button.setDown(false);
    QVERIFY(!button.isDown());
    QVERIFY(!button.state().isPressed());

    QVERIFY(button.stateChangedCount() >= 2);
}

void tst_AbstractButtonState::checkedRequiresCheckable()
{
    TestAbstractButton button;

    button.setChecked(true);
    QVERIFY(!button.isChecked());
    QVERIFY(!button.state().isChecked());

    button.setCheckable(true);
    QVERIFY(button.isCheckable());
    QVERIFY(button.state().isCheckable());

    button.setChecked(true);
    QVERIFY(button.isChecked());
    QVERIFY(button.state().isChecked());

    button.setCheckable(false);
    QVERIFY(!button.isCheckable());
    QVERIFY(!button.state().isCheckable());
    QVERIFY(!button.isChecked());
    QVERIFY(!button.state().isChecked());
}

void tst_AbstractButtonState::leftMousePressAndReleaseSynchronizePressed()
{
    TestAbstractButton button;

    auto press = makeMouseEvent(QEvent::MouseButtonPress,
                                QPoint(10, 10),
                                Qt::LeftButton,
                                Qt::LeftButton);
    QCoreApplication::sendEvent(&button, &press);

    QVERIFY(button.isDown());
    QVERIFY(button.state().isPressed());

    auto release = makeMouseEvent(QEvent::MouseButtonRelease,
                                  QPoint(10, 10),
                                  Qt::LeftButton,
                                  Qt::NoButton);
    QCoreApplication::sendEvent(&button, &release);

    QVERIFY(!button.isDown());
    QVERIFY(!button.state().isPressed());
}

void tst_AbstractButtonState::rightMousePressDoesNotSetPressed()
{
    TestAbstractButton button;

    auto press = makeMouseEvent(QEvent::MouseButtonPress,
                                QPoint(10, 10),
                                Qt::RightButton,
                                Qt::RightButton);
    QCoreApplication::sendEvent(&button, &press);

    QVERIFY(!button.isDown());
    QVERIFY(!button.state().isPressed());
}

void tst_AbstractButtonState::disabledButtonDoesNotEnterPressedState()
{
    TestAbstractButton button;
    button.setEnabled(false);

    QVERIFY(!button.state().isEnabled());

    auto press = makeMouseEvent(QEvent::MouseButtonPress,
                                QPoint(10, 10),
                                Qt::LeftButton,
                                Qt::LeftButton);
    QCoreApplication::sendEvent(&button, &press);

    QVERIFY(!button.isDown());
    QVERIFY(!button.state().isPressed());
}

void tst_AbstractButtonState::focusEventsSynchronizeFocused()
{
    TestAbstractButton button;

    QFocusEvent focusIn(QEvent::FocusIn);
    QCoreApplication::sendEvent(&button, &focusIn);
    QVERIFY(button.state().isFocused());

    QFocusEvent focusOut(QEvent::FocusOut);
    QCoreApplication::sendEvent(&button, &focusOut);
    QVERIFY(!button.state().isFocused());
}

void tst_AbstractButtonState::returnKeyTogglesCheckedWhenCheckable()
{
    TestAbstractButton button;
    button.setCheckable(true);

    QKeyEvent keyPress(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    QCoreApplication::sendEvent(&button, &keyPress);

    QVERIFY(button.isChecked());
    QVERIFY(button.state().isChecked());
    QVERIFY(!button.state().isPressed());

    QKeyEvent keyPress2(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    QCoreApplication::sendEvent(&button, &keyPress2);

    QVERIFY(!button.isChecked());
    QVERIFY(!button.state().isChecked());
}

QTEST_MAIN(tst_AbstractButtonState)
#include "tst_abstractbutton_state.moc"