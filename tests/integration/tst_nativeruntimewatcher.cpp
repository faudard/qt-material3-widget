#include <QtTest/QtTest>

#include <QComboBox>
#include <QCoreApplication>
#include <QLineEdit>
#include <QPointer>
#include <QPushButton>
#include <QSignalSpy>
#include <QStyle>
#include <QSlider>
#include <QStyleFactory>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>

#include "qtmaterial/widgets/inputs/qtmaterialcombobox.h"
#include "qtmaterial/widgets/native/qtmaterialbuttonadapter.h"
#include "qtmaterial/widgets/native/qtmateriallineeditadapter.h"
#include "qtmaterial/widgets/native/qtmaterialnativeadapter.h"

using namespace QtMaterial;

class tst_NativeRuntimeWatcher : public QObject
{
    Q_OBJECT

private slots:
    void declaredOnlyAndPropertyChanges();
    void dynamicallyCreatedControlsAndInternalBarriers();
    void reparentingAndOptOut();
    void overlappingWatchRootsAndManualOwnership();
    void externalStyleReplacementAndSignals();
    void destructionDuringPendingReconciliation();
};

void tst_NativeRuntimeWatcher::declaredOnlyAndPropertyChanges()
{
    QWidget form;
    QPushButton declared(&form);
    QPushButton untouched(&form);
    declared.setProperty(
        QtMaterialNativeAdapter::adaptPropertyName(), true);

    QVERIFY(QtMaterialNativeAdapter::watch(&form));
    QVERIFY(QtMaterialNativeAdapter::isWatched(&form));
    QVERIFY(QtMaterialNativeAdapter::watch(&form)); // idempotent
    QVERIFY(QtMaterialNativeAdapter::isApplied(&declared));
    QVERIFY(!QtMaterialNativeAdapter::isApplied(&untouched));

    untouched.setProperty(
        QtMaterialNativeAdapter::adaptPropertyName(), true);
    QTRY_VERIFY(QtMaterialNativeAdapter::isApplied(&untouched));
    untouched.setProperty(
        QtMaterialNativeAdapter::adaptPropertyName(), false);
    QTRY_VERIFY(!QtMaterialNativeAdapter::isApplied(&untouched));

    QVERIFY(QtMaterialNativeAdapter::unwatch(&form));
    QVERIFY(!QtMaterialNativeAdapter::isApplied(&declared));
    QVERIFY(!QtMaterialNativeAdapter::isApplied(&untouched));
    QCOMPARE(
        declared.property(
            QtMaterialNativeAdapter::adaptPropertyName()).toBool(),
        true);
    QVERIFY(!QtMaterialNativeAdapter::isWatched(&form));
    QVERIFY(!QtMaterialNativeAdapter::unwatch(&form));
}

void tst_NativeRuntimeWatcher::dynamicallyCreatedControlsAndInternalBarriers()
{
    QWidget form;
    QVERIFY(QtMaterialNativeAdapter::watch(
        &form, QtMaterialNativeAdapter::WatchPolicy::AllSupported));

    auto* combo = new QComboBox(&form);
    combo->setEditable(true);
    combo->addItem(QStringLiteral("One"));
    auto* edit = new QLineEdit(&form);
    edit->setClearButtonEnabled(true);
    edit->setText(QStringLiteral("text"));
    auto* button = new QPushButton(&form);

    QTRY_VERIFY(QtMaterialNativeAdapter::isApplied(combo));
    QTRY_VERIFY(QtMaterialNativeAdapter::isApplied(edit));
    QTRY_VERIFY(QtMaterialNativeAdapter::isApplied(button));
    QVERIFY(combo->lineEdit());
    QVERIFY(!QtMaterialNativeAdapter::isApplied(combo->lineEdit()));
    const auto clearButtons = edit->findChildren<QToolButton*>();
    QVERIFY(!clearButtons.isEmpty());
    for (QToolButton* clearButton : clearButtons) {
        QVERIFY(!QtMaterialNativeAdapter::isApplied(clearButton));
    }

    // Qt internals must remain untouched even if explicitly declared.
    combo->lineEdit()->setProperty(
        QtMaterialNativeAdapter::adaptPropertyName(), true);
    QCoreApplication::processEvents();
    QVERIFY(!QtMaterialNativeAdapter::isApplied(combo->lineEdit()));
    QVERIFY(!QtMaterialNativeAdapter::watch(combo->lineEdit()));

    auto* material = new QtMaterialComboBox(&form);
    auto* nativeChild = new QPushButton(material);
    nativeChild->setProperty(
        QtMaterialNativeAdapter::adaptPropertyName(), true);
    QCoreApplication::processEvents();
    QVERIFY(!QtMaterialNativeAdapter::isApplied(nativeChild));

    QPointer<QPushButton> deleted(button);
    delete button;
    QVERIFY(deleted.isNull());
    QCoreApplication::processEvents();

    QVERIFY(QtMaterialNativeAdapter::unwatch(&form));
    QVERIFY(!QtMaterialNativeAdapter::isApplied(combo));
    QVERIFY(!QtMaterialNativeAdapter::isApplied(edit));
}

void tst_NativeRuntimeWatcher::reparentingAndOptOut()
{
    QWidget watchedForm;
    QWidget elsewhere;
    QVERIFY(QtMaterialNativeAdapter::watch(
        &watchedForm,
        QtMaterialNativeAdapter::WatchPolicy::AllSupported));

    auto* button = new QPushButton(&elsewhere);
    QVERIFY(!QtMaterialNativeAdapter::isApplied(button));

    button->setParent(&watchedForm);
    QTRY_VERIFY(QtMaterialNativeAdapter::isApplied(button));

    QtMaterialNativeAdapter::setOptOut(button, true);
    QTRY_VERIFY(!QtMaterialNativeAdapter::isApplied(button));
    QtMaterialNativeAdapter::setOptOut(button, false);
    QTRY_VERIFY(QtMaterialNativeAdapter::isApplied(button));

    button->setParent(&elsewhere);
    QTRY_VERIFY(!QtMaterialNativeAdapter::isApplied(button));
    QVERIFY(QtMaterialNativeAdapter::unwatch(&watchedForm));
}

void tst_NativeRuntimeWatcher::overlappingWatchRootsAndManualOwnership()
{
    QWidget form;
    QWidget panel(&form);
    QPushButton button(&panel);
    QPushButton manual(&form);

    QVERIFY(QtMaterialNativeAdapter::apply(
        &manual,
        QtMaterialNativeAdapter::Options(
            Density::Comfortable, ButtonVariant::Outlined)));

    QtMaterialNativeAdapter::Options parentOptions;
    parentOptions.buttonVariant = ButtonVariant::Text;
    QtMaterialNativeAdapter::Options childOptions;
    childOptions.buttonVariant = ButtonVariant::Filled;

    QVERIFY(QtMaterialNativeAdapter::watch(
        &form,
        QtMaterialNativeAdapter::WatchPolicy::AllSupported,
        parentOptions));
    QVERIFY(QtMaterialNativeAdapter::watch(
        &panel,
        QtMaterialNativeAdapter::WatchPolicy::AllSupported,
        childOptions));
    QCOMPARE(
        int(QtMaterialButtonAdapter::variant(&button)),
        int(ButtonVariant::Filled));

    // An already-adapted native widget is not owned by a form watch.
    QCOMPARE(
        int(QtMaterialButtonAdapter::variant(&manual)),
        int(ButtonVariant::Outlined));

    QVERIFY(QtMaterialNativeAdapter::unwatch(&panel));
    QTRY_COMPARE(
        int(QtMaterialButtonAdapter::variant(&button)),
        int(ButtonVariant::Text));
    QVERIFY(QtMaterialNativeAdapter::isApplied(&button));

    QVERIFY(QtMaterialNativeAdapter::unwatch(&form));
    QVERIFY(!QtMaterialNativeAdapter::isApplied(&button));
    QVERIFY(QtMaterialNativeAdapter::isApplied(&manual));
    QVERIFY(QtMaterialNativeAdapter::remove(&manual));
}

void tst_NativeRuntimeWatcher::externalStyleReplacementAndSignals()
{
    QWidget form;
    auto* button = new QPushButton(&form);
    auto* edit = new QLineEdit(&form);
    auto* combo = new QComboBox(&form);
    combo->addItems({QStringLiteral("A"), QStringLiteral("B")});

    QSignalSpy clicked(button, &QPushButton::clicked);
    QSignalSpy textChanged(edit, &QLineEdit::textChanged);
    QSignalSpy indexChanged(
        combo, QOverload<int>::of(&QComboBox::currentIndexChanged));
    QVERIFY(clicked.isValid());
    QVERIFY(textChanged.isValid());
    QVERIFY(indexChanged.isValid());

    QVERIFY(QtMaterialNativeAdapter::watch(
        &form, QtMaterialNativeAdapter::WatchPolicy::AllSupported));

    QTRY_VERIFY(QtMaterialNativeAdapter::isApplied(button));
    QTRY_VERIFY(QtMaterialNativeAdapter::isApplied(edit));
    QTRY_VERIFY(QtMaterialNativeAdapter::isApplied(combo));

    button->click();
    edit->setText(QStringLiteral("hello"));
    combo->setCurrentIndex(1);
    QCOMPARE(clicked.count(), 1);
    QCOMPARE(textChanged.count(), 1);
    QCOMPARE(indexChanged.count(), 1);

    // An application-driven style replacement must be wrapped again using
    // the new QStyle as the baseline, not a stale prior native style.
    QStyle* external = QStyleFactory::create(QStringLiteral("Fusion"));
    QVERIFY(external);
    // QWidget::setStyle() does not take ownership of the supplied QStyle.
    external->setParent(&form);
    button->setStyle(external);
    QTRY_VERIFY(
        QtMaterialNativeAdapter::isApplied(button)
        && button->style() != external);

    QVERIFY(QtMaterialNativeAdapter::unwatch(&form));
    QVERIFY(!QtMaterialNativeAdapter::isApplied(button));
    QVERIFY(!QtMaterialNativeAdapter::isApplied(edit));
    QVERIFY(!QtMaterialNativeAdapter::isApplied(combo));
    QCOMPARE(button->style(), external);

    button->click();
    edit->setText(QStringLiteral("world"));
    combo->setCurrentIndex(0);
    QCOMPARE(clicked.count(), 2);
    QCOMPARE(textChanged.count(), 2);
    QCOMPARE(indexChanged.count(), 2);
}

void tst_NativeRuntimeWatcher::destructionDuringPendingReconciliation()
{
    auto* form = new QWidget;
    QVERIFY(QtMaterialNativeAdapter::watch(
        form, QtMaterialNativeAdapter::WatchPolicy::AllSupported));
    auto* temporary = new QPushButton(form);
    QPointer<QWidget> root(form);
    QPointer<QPushButton> child(temporary);

    // ChildAdded may still be queued. Neither root nor child can be used
    // by that deferred callback after QObject destruction.
    delete form;
    QVERIFY(root.isNull());
    QVERIFY(child.isNull());
    QCoreApplication::processEvents();

    QWidget fresh;
    QPushButton newButton(&fresh);
    QVERIFY(QtMaterialNativeAdapter::watch(
        &fresh, QtMaterialNativeAdapter::WatchPolicy::AllSupported));
    QVERIFY(QtMaterialNativeAdapter::isApplied(&newButton));
    QVERIFY(QtMaterialNativeAdapter::unwatch(&fresh));
    QVERIFY(!QtMaterialNativeAdapter::isApplied(&newButton));
}

QTEST_MAIN(tst_NativeRuntimeWatcher)
#include "tst_nativeruntimewatcher.moc"
