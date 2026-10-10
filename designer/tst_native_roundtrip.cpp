#include <QtTest/QtTest>

#include <QAbstractButton>
#include <QMetaType>
#include <QPushButton>
#include <QSignalSpy>
#include <QWidget>

#include "qtmaterial/widgets/native/qtmaterialnativeadapter.h"
#include "ui_native_roundtrip.h"

using QtMaterial::QtMaterialNativeAdapter;

class NativeDesignerRoundtripTest : public QObject
{
    Q_OBJECT

private slots:
    void uicKeepsNativeClassesAndTypedDeclarations()
    {
        QWidget form;
        Ui::NativeRoundtripForm ui;
        ui.setupUi(&form);

        QCOMPARE(QString::fromLatin1(ui.pushFilled->metaObject()->className()),
                 QStringLiteral("QPushButton"));
        QCOMPARE(QString::fromLatin1(ui.toolOutlined->metaObject()->className()),
                 QStringLiteral("QToolButton"));
        QCOMPARE(QString::fromLatin1(ui.checkMaterial->metaObject()->className()),
                 QStringLiteral("QCheckBox"));
        QCOMPARE(QString::fromLatin1(ui.radioMaterial->metaObject()->className()),
                 QStringLiteral("QRadioButton"));
        QCOMPARE(QString::fromLatin1(ui.sliderMaterial->metaObject()->className()),
                 QStringLiteral("QSlider"));
        QCOMPARE(QString::fromLatin1(ui.comboMaterial->metaObject()->className()),
                 QStringLiteral("QComboBox"));
        QCOMPARE(QString::fromLatin1(ui.fieldFilled->metaObject()->className()),
                 QStringLiteral("QLineEdit"));
        QCOMPARE(QString::fromLatin1(ui.progressMaterial->metaObject()->className()),
                 QStringLiteral("QProgressBar"));

        const QList<QWidget*> adapted = {
            ui.pushFilled, ui.toolOutlined, ui.checkMaterial,
            ui.radioMaterial, ui.sliderMaterial, ui.comboMaterial,
            ui.fieldFilled, ui.progressMaterial
        };
        for (QWidget* widget : adapted) {
            const QVariant declaration = widget->property("qtm3MaterialAdapt");
            QVERIFY2(declaration.isValid(), qPrintable(widget->objectName()));
            QCOMPARE(declaration.userType(), int(QMetaType::Bool));
            QCOMPARE(declaration.toBool(), true);
        }
        QCOMPARE(ui.pushFilled->property("qtm3MaterialVariant").toString(),
                 QStringLiteral("filled"));
        QCOMPARE(ui.toolOutlined->property("qtm3MaterialVariant").toString(),
                 QStringLiteral("outlined"));
        QCOMPARE(ui.pushFilled->property("qtm3MaterialVariant").userType(),
                 int(QMetaType::QString));
        QCOMPARE(ui.pushFilled->property("qtm3MaterialDensity").toString(),
                 QStringLiteral("compact"));
        QCOMPARE(ui.toolOutlined->property("qtm3MaterialDensity").toString(),
                 QStringLiteral("comfortable"));
        QCOMPARE(ui.fieldFilled->property("qtm3MaterialTextFieldVariant").toString(),
                 QStringLiteral("filled"));
        QCOMPARE(ui.fieldFilled->property("qtm3MaterialTextFieldVariant").userType(),
                 int(QMetaType::QString));
        QVERIFY(ui.optedOutButton->property("qtm3MaterialAdapt").toBool());
        QVERIFY(ui.optedOutButton->property("qtm3MaterialOptOut").toBool());
        QCOMPARE(ui.optedOutButton->property("qtm3MaterialOptOut").userType(),
                 int(QMetaType::Bool));
        QVERIFY(!ui.undeclaredButton->property("qtm3MaterialAdapt").isValid());
        QCOMPARE(ui.sliderMaterial->value(), 31);
        QCOMPARE(ui.comboMaterial->count(), 2);
        QCOMPARE(ui.progressMaterial->value(), 56);
    }

    void runtimeAppliesOnlyDeclaredAndRestoresNativeControls()
    {
        QWidget form;
        Ui::NativeRoundtripForm ui;
        ui.setupUi(&form);

        QCOMPARE(QtMaterialNativeAdapter::applyDeclaredToDescendants(&form), 8);
        const QList<QWidget*> adapted = {
            ui.pushFilled, ui.toolOutlined, ui.checkMaterial,
            ui.radioMaterial, ui.sliderMaterial, ui.comboMaterial,
            ui.fieldFilled, ui.progressMaterial
        };
        for (QWidget* widget : adapted) {
            QVERIFY2(QtMaterialNativeAdapter::isApplied(widget),
                     qPrintable(widget->objectName()));
        }
        QVERIFY(!QtMaterialNativeAdapter::isApplied(ui.optedOutButton));
        QVERIFY(!QtMaterialNativeAdapter::isApplied(ui.undeclaredButton));

        QSignalSpy buttonClicks(ui.pushFilled, &QAbstractButton::clicked);
        QVERIFY(buttonClicks.isValid());
        ui.pushFilled->click();
        QCOMPARE(buttonClicks.count(), 1);
        QCOMPARE(ui.comboMaterial->itemText(1), QStringLiteral("Beta"));

        // The pass is safe to repeat, and removing adaptations is reversible.
        QCOMPARE(QtMaterialNativeAdapter::applyDeclaredToDescendants(&form), 8);
        QCOMPARE(QtMaterialNativeAdapter::removeFromDescendants(&form), 8);
        for (QWidget* widget : adapted) {
            QVERIFY2(!QtMaterialNativeAdapter::isApplied(widget),
                     qPrintable(widget->objectName()));
        }
        QCOMPARE(QtMaterialNativeAdapter::removeFromDescendants(&form), 0);
        QCOMPARE(QtMaterialNativeAdapter::applyDeclaredToDescendants(&form), 8);
        QCOMPARE(QtMaterialNativeAdapter::removeFromDescendants(&form), 8);
    }
};

QTEST_MAIN(NativeDesignerRoundtripTest)
#include "tst_native_roundtrip.moc"
