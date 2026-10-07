#include <QtTest/QtTest>

#include <QPushButton>
#include <QPixmap>
#include <QStyle>
#include <QVBoxLayout>
#include <QWidget>

#include "qtmaterial/widgets/native/qtmaterialbuttonadapter.h"

using namespace QtMaterial;

class tst_NativeButtonAdapter : public QObject
{
    Q_OBJECT

private slots:
    void preservesQPushButtonContract()
    {
        QPushButton button(QStringLiteral("Save"));
        QStyle* originalStyle = button.style();
        button.setCheckable(true);

        QtMaterialButtonAdapter::apply(
            &button,
            ButtonVariant::Filled,
            Density::Default);

        QVERIFY(QtMaterialButtonAdapter::isApplied(&button));
        QVERIFY(button.style() != originalStyle);
        QCOMPARE(
            int(QtMaterialButtonAdapter::variant(&button)),
            int(ButtonVariant::Filled));

        QSignalSpy clicked(&button, &QPushButton::clicked);
        button.click();
        QCOMPARE(clicked.count(), 1);
        QVERIFY(button.isChecked());

        QtMaterialButtonAdapter::remove(&button);
        QVERIFY(!QtMaterialButtonAdapter::isApplied(&button));
        QCOMPARE(button.style(), originalStyle);
    }

    void variantsAndDensityRoundTrip()
    {
        QPushButton button(QStringLiteral("Action"));
        QtMaterialButtonAdapter::apply(&button);

        const ButtonVariant variants[] = {
            ButtonVariant::Text,
            ButtonVariant::Filled,
            ButtonVariant::FilledTonal,
            ButtonVariant::Outlined,
            ButtonVariant::Elevated
        };
        for (ButtonVariant value : variants) {
            QtMaterialButtonAdapter::setVariant(&button, value);
            QCOMPARE(
                int(QtMaterialButtonAdapter::variant(&button)),
                int(value));
        }

        QtMaterialButtonAdapter::setDensity(
            &button, Density::Compact);
        QCOMPARE(
            int(QtMaterialButtonAdapter::density(&button)),
            int(Density::Compact));
        QtMaterialButtonAdapter::setDensity(
            &button, Density::Comfortable);
        QCOMPARE(
            int(QtMaterialButtonAdapter::density(&button)),
            int(Density::Comfortable));
    }

    void dynamicPropertiesAreLive()
    {
        QPushButton button(QStringLiteral("Dynamic"));
        QtMaterialButtonAdapter::apply(&button);

        button.setProperty(
            QtMaterialButtonAdapter::variantPropertyName(),
            QStringLiteral("outlined"));
        QCOMPARE(
            int(QtMaterialButtonAdapter::variant(&button)),
            int(ButtonVariant::Outlined));

        button.setProperty(
            QtMaterialButtonAdapter::densityPropertyName(),
            QStringLiteral("compact"));
        QCOMPARE(
            int(QtMaterialButtonAdapter::density(&button)),
            int(Density::Compact));
    }

    void adaptsWidgetTreeAndHonorsOptOut()
    {
        QWidget root;
        auto* layout = new QVBoxLayout(&root);
        auto* first = new QPushButton(QStringLiteral("First"), &root);
        auto* second = new QPushButton(QStringLiteral("Second"), &root);
        layout->addWidget(first);
        layout->addWidget(second);

        QtMaterialButtonAdapter::setOptOut(second, true);
        QCOMPARE(
            QtMaterialButtonAdapter::applyToDescendants(
                &root, ButtonVariant::FilledTonal),
            1);
        QVERIFY(QtMaterialButtonAdapter::isApplied(first));
        QVERIFY(!QtMaterialButtonAdapter::isApplied(second));
    }

    void adaptedButtonRenders()
    {
        QPushButton button(QStringLiteral("Render"));
        button.resize(180, 56);
        QtMaterialButtonAdapter::apply(
            &button, ButtonVariant::Outlined);

        QPixmap image(button.size());
        image.fill(Qt::transparent);
        button.render(&image);
        QVERIFY(!image.isNull());
    }
};

QTEST_MAIN(tst_NativeButtonAdapter)
#include "tst_nativebuttonadapter.moc"
