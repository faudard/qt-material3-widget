#include <QtTest/QtTest>

#include <QCheckBox>
#include <QPixmap>
#include <QRadioButton>
#include <QSignalSpy>
#include <QStyle>
#include <QVBoxLayout>
#include <QWidget>

#include "qtmaterial/widgets/native/qtmaterialselectionadapter.h"

using namespace QtMaterial;

class tst_NativeSelectionAdapter : public QObject
{
    Q_OBJECT

private slots:
    void checkboxPreservesNativeContract()
    {
        QCheckBox checkbox(QStringLiteral("Remember me"));
        QStyle* originalStyle = checkbox.style();
        checkbox.setTristate(true);

        QtMaterialSelectionAdapter::apply(
            &checkbox,
            Density::Compact);

        QVERIFY(QtMaterialSelectionAdapter::isApplied(&checkbox));
        QVERIFY(checkbox.style() != originalStyle);
        QVERIFY(checkbox.isTristate());
        QCOMPARE(
            int(QtMaterialSelectionAdapter::density(&checkbox)),
            int(Density::Compact));

        QSignalSpy toggled(&checkbox, &QAbstractButton::toggled);
        checkbox.setCheckState(Qt::PartiallyChecked);
        QCOMPARE(checkbox.checkState(), Qt::PartiallyChecked);
        QVERIFY(checkbox.isChecked());
        QVERIFY(toggled.count() >= 1);

        QtMaterialSelectionAdapter::remove(&checkbox);
        QVERIFY(!QtMaterialSelectionAdapter::isApplied(&checkbox));
        QCOMPARE(checkbox.style(), originalStyle);
        QVERIFY(checkbox.isTristate());
        QCOMPARE(checkbox.checkState(), Qt::PartiallyChecked);
    }

    void radioPreservesAutoExclusiveContract()
    {
        QWidget parent;
        auto* first =
            new QRadioButton(QStringLiteral("One"), &parent);
        auto* second =
            new QRadioButton(QStringLiteral("Two"), &parent);

        QVERIFY(first->autoExclusive());
        QVERIFY(second->autoExclusive());

        QtMaterialSelectionAdapter::apply(first);
        QtMaterialSelectionAdapter::apply(second);

        first->click();
        QVERIFY(first->isChecked());
        QVERIFY(!second->isChecked());

        second->click();
        QVERIFY(!first->isChecked());
        QVERIFY(second->isChecked());
        QVERIFY(first->autoExclusive());
        QVERIFY(second->autoExclusive());
    }

    void densityPropertyIsLive()
    {
        QCheckBox checkbox(QStringLiteral("Compact"));
        QtMaterialSelectionAdapter::apply(&checkbox);

        checkbox.setProperty(
            QtMaterialSelectionAdapter::densityPropertyName(),
            QStringLiteral("comfortable"));
        QCOMPARE(
            int(QtMaterialSelectionAdapter::density(&checkbox)),
            int(Density::Comfortable));

        QtMaterialSelectionAdapter::setDensity(
            &checkbox,
            Density::Compact);
        QCOMPARE(
            int(QtMaterialSelectionAdapter::density(&checkbox)),
            int(Density::Compact));
    }

    void adaptsTreeAndHonorsOptOut()
    {
        QWidget root;
        auto* layout = new QVBoxLayout(&root);
        auto* checkbox =
            new QCheckBox(QStringLiteral("Checkbox"), &root);
        auto* radio =
            new QRadioButton(QStringLiteral("Radio"), &root);
        auto* excluded =
            new QCheckBox(QStringLiteral("Excluded"), &root);
        layout->addWidget(checkbox);
        layout->addWidget(radio);
        layout->addWidget(excluded);

        QtMaterialSelectionAdapter::setOptOut(
            excluded,
            true);

        QCOMPARE(
            QtMaterialSelectionAdapter::applyToDescendants(
                &root,
                Density::Default),
            2);
        QVERIFY(QtMaterialSelectionAdapter::isApplied(checkbox));
        QVERIFY(QtMaterialSelectionAdapter::isApplied(radio));
        QVERIFY(!QtMaterialSelectionAdapter::isApplied(excluded));
    }

    void rendersCheckboxAndRadio()
    {
        QCheckBox checkbox(QStringLiteral("Check"));
        checkbox.setCheckState(Qt::Checked);
        checkbox.resize(180, 52);
        QtMaterialSelectionAdapter::apply(&checkbox);

        QPixmap checkboxImage(checkbox.size());
        checkboxImage.fill(Qt::transparent);
        checkbox.render(&checkboxImage);
        QVERIFY(!checkboxImage.isNull());

        QRadioButton radio(QStringLiteral("Radio"));
        radio.setChecked(true);
        radio.resize(180, 52);
        QtMaterialSelectionAdapter::apply(&radio);

        QPixmap radioImage(radio.size());
        radioImage.fill(Qt::transparent);
        radio.render(&radioImage);
        QVERIFY(!radioImage.isNull());
    }

    void disabledAndRtlRender()
    {
        QCheckBox checkbox(QStringLiteral("RTL"));
        checkbox.setLayoutDirection(Qt::RightToLeft);
        checkbox.setChecked(true);
        checkbox.setEnabled(false);
        checkbox.resize(180, 52);
        QtMaterialSelectionAdapter::apply(&checkbox);

        QPixmap image(checkbox.size());
        image.fill(Qt::transparent);
        checkbox.render(&image);
        QVERIFY(!image.isNull());
    }
};

QTEST_MAIN(tst_NativeSelectionAdapter)
#include "tst_nativeselectionadapter.moc"
