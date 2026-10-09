#include <QtTest/QtTest>

#include <QAction>
#include <QMenu>
#include <QSignalSpy>
#include <QStyle>
#include <QStyleOption>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>

#include "qtmaterial/widgets/native/qtmaterialtoolbuttonadapter.h"

using namespace QtMaterial;

namespace {

class ExposedToolButton final : public QToolButton
{
public:
    using QToolButton::initStyleOption;
};

} // namespace

class tst_NativeToolButtonAdapter : public QObject
{
    Q_OBJECT

private slots:
    void preservesNativeContracts();
    void splitMenuGeometryTracksRtl();
    void variantAndDensityAreLive();
    void adaptsTreeAndHonorsOptOut();
    void restoresStyle();
};

void tst_NativeToolButtonAdapter::preservesNativeContracts()
{
    ExposedToolButton button;
    QAction action(QStringLiteral("Run"), &button);
    action.setCheckable(true);

    QMenu menu(&button);
    menu.addAction(QStringLiteral("Alternative"));

    button.setDefaultAction(&action);
    button.setMenu(&menu);
    button.setPopupMode(QToolButton::MenuButtonPopup);
    button.setAutoRaise(true);
    button.setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    QSignalSpy clicked(
        &button,
        &QToolButton::clicked);

    QtMaterialToolButtonAdapter::apply(
        &button,
        ButtonVariant::FilledTonal,
        Density::Default);

    QVERIFY(
        QtMaterialToolButtonAdapter::isApplied(
            &button));
    QCOMPARE(button.defaultAction(), &action);
    QCOMPARE(button.menu(), &menu);
    QCOMPARE(
        button.popupMode(),
        QToolButton::MenuButtonPopup);
    QVERIFY(button.autoRaise());
    QCOMPARE(
        button.toolButtonStyle(),
        Qt::ToolButtonTextBesideIcon);
    QVERIFY(button.isCheckable());

    button.click();
    QCOMPARE(clicked.count(), 1);
    QVERIFY(button.isChecked());
}

void tst_NativeToolButtonAdapter::
    splitMenuGeometryTracksRtl()
{
    ExposedToolButton button;
    QMenu menu(&button);
    menu.addAction(QStringLiteral("One"));
    button.setText(QStringLiteral("Actions"));
    button.setMenu(&menu);
    button.setPopupMode(QToolButton::MenuButtonPopup);
    button.resize(220, 48);

    QtMaterialToolButtonAdapter::apply(&button);

    QStyleOptionToolButton option;
    button.initStyleOption(&option);

    const QRect ltrMain =
        button.style()->subControlRect(
            QStyle::CC_ToolButton,
            &option,
            QStyle::SC_ToolButton,
            &button);
    const QRect ltrMenu =
        button.style()->subControlRect(
            QStyle::CC_ToolButton,
            &option,
            QStyle::SC_ToolButtonMenu,
            &button);

    QVERIFY(!ltrMain.isEmpty());
    QVERIFY(!ltrMenu.isEmpty());
    QVERIFY(ltrMenu.center().x() > ltrMain.center().x());
    QCOMPARE(
        int(button.style()->hitTestComplexControl(
            QStyle::CC_ToolButton,
            &option,
            ltrMenu.center(),
            &button)),
        int(QStyle::SC_ToolButtonMenu));

    button.setLayoutDirection(Qt::RightToLeft);
    button.initStyleOption(&option);

    const QRect rtlMain =
        button.style()->subControlRect(
            QStyle::CC_ToolButton,
            &option,
            QStyle::SC_ToolButton,
            &button);
    const QRect rtlMenu =
        button.style()->subControlRect(
            QStyle::CC_ToolButton,
            &option,
            QStyle::SC_ToolButtonMenu,
            &button);

    QVERIFY(rtlMenu.center().x() < rtlMain.center().x());
    QCOMPARE(
        int(button.style()->hitTestComplexControl(
            QStyle::CC_ToolButton,
            &option,
            rtlMenu.center(),
            &button)),
        int(QStyle::SC_ToolButtonMenu));
}

void tst_NativeToolButtonAdapter::
    variantAndDensityAreLive()
{
    QToolButton button;
    QtMaterialToolButtonAdapter::apply(&button);

    QtMaterialToolButtonAdapter::setVariant(
        &button,
        ButtonVariant::Outlined);
    QCOMPARE(
        int(QtMaterialToolButtonAdapter::variant(&button)),
        int(ButtonVariant::Outlined));

    button.setProperty(
        QtMaterialToolButtonAdapter::
            densityPropertyName(),
        QStringLiteral("compact"));
    QCOMPARE(
        int(QtMaterialToolButtonAdapter::density(&button)),
        int(Density::Compact));

    QtMaterialToolButtonAdapter::setDensity(
        &button,
        Density::Comfortable);
    QCOMPARE(
        int(QtMaterialToolButtonAdapter::density(&button)),
        int(Density::Comfortable));
}

void tst_NativeToolButtonAdapter::
    adaptsTreeAndHonorsOptOut()
{
    QWidget root;
    auto* layout = new QVBoxLayout(&root);
    auto* first = new QToolButton(&root);
    auto* second = new QToolButton(&root);
    layout->addWidget(first);
    layout->addWidget(second);

    QtMaterialToolButtonAdapter::setOptOut(
        second,
        true);

    QCOMPARE(
        QtMaterialToolButtonAdapter::applyToDescendants(
            &root,
            ButtonVariant::Text,
            Density::Compact),
        1);
    QVERIFY(
        QtMaterialToolButtonAdapter::isApplied(first));
    QVERIFY(
        !QtMaterialToolButtonAdapter::isApplied(second));
}

void tst_NativeToolButtonAdapter::restoresStyle()
{
    QToolButton button;
    QStyle* originalStyle = button.style();

    QtMaterialToolButtonAdapter::apply(
        &button,
        ButtonVariant::Filled);

    QVERIFY(button.style() != originalStyle);

    QtMaterialToolButtonAdapter::remove(&button);

    QVERIFY(
        !QtMaterialToolButtonAdapter::isApplied(
            &button));
    QCOMPARE(button.style(), originalStyle);
}

QTEST_MAIN(tst_NativeToolButtonAdapter)
#include "tst_nativetoolbuttonadapter.moc"
