#include <QtTest/QtTest>

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>

#include "qtmaterial/widgets/inputs/qtmaterialcombobox.h"
#include "qtmaterial/widgets/inputs/qtmaterialslider.h"
#include "qtmaterial/widgets/native/qtmaterialbuttonadapter.h"
#include "qtmaterial/widgets/native/qtmaterialcomboboxadapter.h"
#include "qtmaterial/widgets/native/qtmateriallineeditadapter.h"
#include "qtmaterial/widgets/native/qtmaterialnativeadapter.h"
#include "qtmaterial/widgets/native/qtmaterialprogressbaradapter.h"
#include "qtmaterial/widgets/native/qtmaterialselectionadapter.h"
#include "qtmaterial/widgets/native/qtmaterialslideradapter.h"
#include "qtmaterial/widgets/native/qtmaterialtoolbuttonadapter.h"

using namespace QtMaterial;

class tst_NativeAdapterFacade : public QObject
{
    Q_OBJECT

private slots:
    void classifiesAndDispatchesSupportedWidgets();
    void propagatesOptions();
    void traversalUsesNativeControlBarriers();
    void skipsFirstClassMaterialWidgets();
    void optOutAndRemoveTreeAreUnified();
};

void tst_NativeAdapterFacade::
    classifiesAndDispatchesSupportedWidgets()
{
    QPushButton push;
    QToolButton tool;
    QCheckBox check;
    QRadioButton radio;
    QSlider slider;
    QComboBox combo;
    QLineEdit lineEdit;
    QProgressBar progress;
    QLabel unsupported;

    struct Case
    {
        QWidget* widget;
        QtMaterialNativeAdapter::WidgetKind kind;
    };

    const Case cases[] = {
        { &push, QtMaterialNativeAdapter::WidgetKind::PushButton },
        { &tool, QtMaterialNativeAdapter::WidgetKind::ToolButton },
        { &check, QtMaterialNativeAdapter::WidgetKind::CheckBox },
        { &radio, QtMaterialNativeAdapter::WidgetKind::RadioButton },
        { &slider, QtMaterialNativeAdapter::WidgetKind::Slider },
        { &combo, QtMaterialNativeAdapter::WidgetKind::ComboBox },
        { &lineEdit, QtMaterialNativeAdapter::WidgetKind::LineEdit },
        { &progress, QtMaterialNativeAdapter::WidgetKind::ProgressBar }
    };

    for (const Case& item : cases) {
        QCOMPARE(
            int(QtMaterialNativeAdapter::kind(item.widget)),
            int(item.kind));
        QVERIFY(
            QtMaterialNativeAdapter::isSupported(item.widget));
        QVERIFY(
            QtMaterialNativeAdapter::apply(item.widget));
        QVERIFY(
            QtMaterialNativeAdapter::isApplied(item.widget));
        QVERIFY(
            QtMaterialNativeAdapter::remove(item.widget));
        QVERIFY(
            !QtMaterialNativeAdapter::isApplied(item.widget));
    }

    QCOMPARE(
        int(QtMaterialNativeAdapter::kind(&unsupported)),
        int(QtMaterialNativeAdapter::WidgetKind::Unsupported));
    QVERIFY(
        !QtMaterialNativeAdapter::isSupported(&unsupported));
    QVERIFY(
        !QtMaterialNativeAdapter::apply(&unsupported));
}

void tst_NativeAdapterFacade::propagatesOptions()
{
    QPushButton push;
    QToolButton tool;
    QLineEdit lineEdit;
    QSlider slider;

    QtMaterialNativeAdapter::Options options;
    options.density = Density::Compact;
    options.buttonVariant = ButtonVariant::FilledTonal;
    options.textFieldVariant =
        QtMaterialNativeAdapter::TextFieldVariant::Filled;

    QVERIFY(QtMaterialNativeAdapter::apply(&push, options));
    QVERIFY(QtMaterialNativeAdapter::apply(&tool, options));
    QVERIFY(QtMaterialNativeAdapter::apply(&lineEdit, options));
    QVERIFY(QtMaterialNativeAdapter::apply(&slider, options));

    QCOMPARE(
        int(QtMaterialButtonAdapter::variant(&push)),
        int(ButtonVariant::FilledTonal));
    QCOMPARE(
        int(QtMaterialToolButtonAdapter::variant(&tool)),
        int(ButtonVariant::FilledTonal));
    QCOMPARE(
        int(QtMaterialLineEditAdapter::variant(&lineEdit)),
        int(QtMaterialLineEditAdapter::Variant::Filled));

    QCOMPARE(
        int(QtMaterialButtonAdapter::density(&push)),
        int(Density::Compact));
    QCOMPARE(
        int(QtMaterialToolButtonAdapter::density(&tool)),
        int(Density::Compact));
    QCOMPARE(
        int(QtMaterialLineEditAdapter::density(&lineEdit)),
        int(Density::Compact));
    QCOMPARE(
        int(QtMaterialSliderAdapter::density(&slider)),
        int(Density::Compact));
}

void tst_NativeAdapterFacade::
    traversalUsesNativeControlBarriers()
{
    QWidget root;
    auto* layout = new QVBoxLayout(&root);

    auto* combo = new QComboBox(&root);
    combo->setEditable(true);
    combo->addItem(QStringLiteral("One"));
    layout->addWidget(combo);

    auto* lineEdit = new QLineEdit(&root);
    lineEdit->setClearButtonEnabled(true);
    lineEdit->setText(QStringLiteral("value"));
    layout->addWidget(lineEdit);

    auto* button = new QPushButton(
        QStringLiteral("Apply"),
        &root);
    layout->addWidget(button);

    QCOMPARE(
        QtMaterialNativeAdapter::applyToDescendants(&root),
        3);

    QVERIFY(
        QtMaterialComboBoxAdapter::isApplied(combo));
    QVERIFY(
        QtMaterialLineEditAdapter::isApplied(lineEdit));
    QVERIFY(
        QtMaterialButtonAdapter::isApplied(button));

    // The editable combo's implementation QLineEdit is below a supported
    // native control and must not be adapted by the facade traversal.
    QVERIFY(combo->lineEdit());
    QVERIFY(
        !QtMaterialLineEditAdapter::isApplied(
            combo->lineEdit()));
}

void tst_NativeAdapterFacade::
    skipsFirstClassMaterialWidgets()
{
    QtMaterialSlider materialSlider;
    QtMaterialComboBox materialCombo;
    materialCombo.setEditable(true);

    QCOMPARE(
        int(QtMaterialNativeAdapter::kind(&materialSlider)),
        int(QtMaterialNativeAdapter::WidgetKind::Unsupported));
    QCOMPARE(
        int(QtMaterialNativeAdapter::kind(&materialCombo)),
        int(QtMaterialNativeAdapter::WidgetKind::Unsupported));

    QVERIFY(
        !QtMaterialNativeAdapter::apply(&materialSlider));
    QVERIFY(
        !QtMaterialNativeAdapter::apply(&materialCombo));

    QWidget root;
    auto* layout = new QVBoxLayout(&root);
    layout->addWidget(new QtMaterialSlider(&root));
    auto* combo = new QtMaterialComboBox(&root);
    combo->setEditable(true);
    layout->addWidget(combo);

    QCOMPARE(
        QtMaterialNativeAdapter::applyToDescendants(&root),
        0);
    if (combo->lineEdit()) {
        QVERIFY(
            !QtMaterialLineEditAdapter::isApplied(
                combo->lineEdit()));
    }
}

void tst_NativeAdapterFacade::
    optOutAndRemoveTreeAreUnified()
{
    QWidget root;
    auto* layout = new QVBoxLayout(&root);
    auto* push = new QPushButton(&root);
    auto* slider = new QSlider(&root);
    auto* progress = new QProgressBar(&root);
    layout->addWidget(push);
    layout->addWidget(slider);
    layout->addWidget(progress);

    QtMaterialNativeAdapter::setOptOut(slider, true);
    QVERIFY(
        QtMaterialNativeAdapter::isOptedOut(slider));

    QCOMPARE(
        QtMaterialNativeAdapter::applyToDescendants(&root),
        2);
    QVERIFY(
        QtMaterialNativeAdapter::isApplied(push));
    QVERIFY(
        !QtMaterialNativeAdapter::isApplied(slider));
    QVERIFY(
        QtMaterialNativeAdapter::isApplied(progress));

    QCOMPARE(
        QtMaterialNativeAdapter::removeFromDescendants(&root),
        2);
    QVERIFY(
        !QtMaterialNativeAdapter::isApplied(push));
    QVERIFY(
        !QtMaterialNativeAdapter::isApplied(progress));

    QtMaterialNativeAdapter::setOptOut(slider, false);
    QVERIFY(
        !QtMaterialNativeAdapter::isOptedOut(slider));
    QVERIFY(
        QtMaterialNativeAdapter::apply(slider));
}

QTEST_MAIN(tst_NativeAdapterFacade)
#include "tst_nativeadapterfacade.moc"
