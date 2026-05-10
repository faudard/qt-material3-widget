#include <QtTest/QtTest>

#include <QIcon>
#include <QPainter>
#include <QPixmap>

#include "qtmaterial/widgets/buttons/qtmaterialelevatedbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledtonalbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"

namespace {

QIcon contractIcon()
{
    QPixmap pixmap(24, 24);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.drawEllipse(QRectF(4, 4, 16, 16));
    painter.end();

    return QIcon(pixmap);
}

} // namespace

class tst_ButtonConstructorApi : public QObject
{
    Q_OBJECT

private slots:
    void textConstructorsSetText();
    void iconTextConstructorsSetIconAndText();
    void constructorButtonsHaveAccessibleNamesFromText();
    void constructorButtonsHaveValidSizeHints();
};

void tst_ButtonConstructorApi::textConstructorsSetText()
{
    QtMaterial::QtMaterialTextButton text(QStringLiteral("Text"));
    QtMaterial::QtMaterialFilledButton filled(QStringLiteral("Filled"));
    QtMaterial::QtMaterialFilledTonalButton tonal(QStringLiteral("Tonal"));
    QtMaterial::QtMaterialOutlinedButton outlined(QStringLiteral("Outlined"));
    QtMaterial::QtMaterialElevatedButton elevated(QStringLiteral("Elevated"));

    QCOMPARE(text.text(), QStringLiteral("Text"));
    QCOMPARE(filled.text(), QStringLiteral("Filled"));
    QCOMPARE(tonal.text(), QStringLiteral("Tonal"));
    QCOMPARE(outlined.text(), QStringLiteral("Outlined"));
    QCOMPARE(elevated.text(), QStringLiteral("Elevated"));
}

void tst_ButtonConstructorApi::iconTextConstructorsSetIconAndText()
{
    const QIcon icon = contractIcon();

    QtMaterial::QtMaterialTextButton text(icon, QStringLiteral("Text"));
    QtMaterial::QtMaterialFilledButton filled(icon, QStringLiteral("Filled"));
    QtMaterial::QtMaterialFilledTonalButton tonal(icon, QStringLiteral("Tonal"));
    QtMaterial::QtMaterialOutlinedButton outlined(icon, QStringLiteral("Outlined"));
    QtMaterial::QtMaterialElevatedButton elevated(icon, QStringLiteral("Elevated"));

    QCOMPARE(text.text(), QStringLiteral("Text"));
    QCOMPARE(filled.text(), QStringLiteral("Filled"));
    QCOMPARE(tonal.text(), QStringLiteral("Tonal"));
    QCOMPARE(outlined.text(), QStringLiteral("Outlined"));
    QCOMPARE(elevated.text(), QStringLiteral("Elevated"));

    QVERIFY(!text.icon().isNull());
    QVERIFY(!filled.icon().isNull());
    QVERIFY(!tonal.icon().isNull());
    QVERIFY(!outlined.icon().isNull());
    QVERIFY(!elevated.icon().isNull());
}

void tst_ButtonConstructorApi::constructorButtonsHaveAccessibleNamesFromText()
{
    const QIcon icon = contractIcon();

    QtMaterial::QtMaterialTextButton text(icon, QStringLiteral("Text"));
    QtMaterial::QtMaterialFilledButton filled(icon, QStringLiteral("Filled"));
    QtMaterial::QtMaterialFilledTonalButton tonal(icon, QStringLiteral("Tonal"));
    QtMaterial::QtMaterialOutlinedButton outlined(icon, QStringLiteral("Outlined"));
    QtMaterial::QtMaterialElevatedButton elevated(icon, QStringLiteral("Elevated"));

    QCOMPARE(text.accessibleName(), QStringLiteral("Text"));
    QCOMPARE(filled.accessibleName(), QStringLiteral("Filled"));
    QCOMPARE(tonal.accessibleName(), QStringLiteral("Tonal"));
    QCOMPARE(outlined.accessibleName(), QStringLiteral("Outlined"));
    QCOMPARE(elevated.accessibleName(), QStringLiteral("Elevated"));
}

void tst_ButtonConstructorApi::constructorButtonsHaveValidSizeHints()
{
    const QIcon icon = contractIcon();

    QtMaterial::QtMaterialTextButton text(icon, QStringLiteral("Text"));
    QtMaterial::QtMaterialFilledButton filled(icon, QStringLiteral("Filled"));
    QtMaterial::QtMaterialFilledTonalButton tonal(icon, QStringLiteral("Tonal"));
    QtMaterial::QtMaterialOutlinedButton outlined(icon, QStringLiteral("Outlined"));
    QtMaterial::QtMaterialElevatedButton elevated(icon, QStringLiteral("Elevated"));

    QVERIFY(text.sizeHint().isValid());
    QVERIFY(filled.sizeHint().isValid());
    QVERIFY(tonal.sizeHint().isValid());
    QVERIFY(outlined.sizeHint().isValid());
    QVERIFY(elevated.sizeHint().isValid());
}

QTEST_MAIN(tst_ButtonConstructorApi)
#include "tst_button_constructor_api.moc"
