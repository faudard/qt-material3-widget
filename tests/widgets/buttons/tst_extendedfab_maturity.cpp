#include <QtTest/QtTest>

#include <QIcon>
#include <QPainter>
#include <QPixmap>

#include "qtmaterial/widgets/buttons/qtmaterialextendedfab.h"

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

class tst_ExtendedFabMaturity : public QObject
{
    Q_OBJECT

private slots:
    void textConstructorAppliesText();
    void iconTextConstructorAppliesIconAndText();
    void visibleTextProvidesAccessibleName();
    void visibleTextChangeUpdatesAutoAccessibleName();
    void explicitAccessibleNameIsPreserved();
    void mnemonicMarkersAreRemovedFromAutoAccessibleName();
    void tooltipProvidesAccessibleDescription();
    void sizeHintIncludesIconTextAndTouchTarget();
    void materialMetadataIsExplicit();
};

void tst_ExtendedFabMaturity::textConstructorAppliesText()
{
    QtMaterial::QtMaterialExtendedFab button(QStringLiteral("Create"));
    QCOMPARE(button.text(), QStringLiteral("Create"));
    QVERIFY(button.icon().isNull());
}

void tst_ExtendedFabMaturity::iconTextConstructorAppliesIconAndText()
{
    QtMaterial::QtMaterialExtendedFab button(contractIcon(), QStringLiteral("Create"));
    QCOMPARE(button.text(), QStringLiteral("Create"));
    QVERIFY(!button.icon().isNull());
}

void tst_ExtendedFabMaturity::visibleTextProvidesAccessibleName()
{
    QtMaterial::QtMaterialExtendedFab button(QStringLiteral("Create"));
    QCOMPARE(button.effectiveAccessibleName(), QStringLiteral("Create"));
    QCOMPARE(button.accessibleName(), QStringLiteral("Create"));
    QVERIFY(button.hasUsableAccessibleName());
}

void tst_ExtendedFabMaturity::visibleTextChangeUpdatesAutoAccessibleName()
{
    QtMaterial::QtMaterialExtendedFab button(QStringLiteral("Create"));
    QCOMPARE(button.accessibleName(), QStringLiteral("Create"));

    button.setText(QStringLiteral("Add"));
    QCOMPARE(button.accessibleName(), QStringLiteral("Add"));
    QCOMPARE(button.effectiveAccessibleName(), QStringLiteral("Add"));
}

void tst_ExtendedFabMaturity::explicitAccessibleNameIsPreserved()
{
    QtMaterial::QtMaterialExtendedFab button(QStringLiteral("Create"));
    button.setAccessibleName(QStringLiteral("Create project"));
    button.setText(QStringLiteral("Add"));

    QCOMPARE(button.accessibleName(), QStringLiteral("Create project"));
    QCOMPARE(button.effectiveAccessibleName(), QStringLiteral("Create project"));
}

void tst_ExtendedFabMaturity::mnemonicMarkersAreRemovedFromAutoAccessibleName()
{
    QtMaterial::QtMaterialExtendedFab button(QStringLiteral("&Create"));
    QCOMPARE(button.accessibleName(), QStringLiteral("Create"));
    QCOMPARE(button.effectiveAccessibleName(), QStringLiteral("Create"));
}

void tst_ExtendedFabMaturity::tooltipProvidesAccessibleDescription()
{
    QtMaterial::QtMaterialExtendedFab button(QStringLiteral("Create"));
    button.setToolTip(QStringLiteral("Create a new project"));
    QCOMPARE(button.accessibleDescription(), QStringLiteral("Create a new project"));

    button.setToolTip(QStringLiteral("Create item"));
    QCOMPARE(button.accessibleDescription(), QStringLiteral("Create item"));
}

void tst_ExtendedFabMaturity::sizeHintIncludesIconTextAndTouchTarget()
{
    QtMaterial::QtMaterialExtendedFab textOnly(QStringLiteral("Create"));
    QtMaterial::QtMaterialExtendedFab iconText(contractIcon(), QStringLiteral("Create"));

    QVERIFY(textOnly.sizeHint().isValid());
    QVERIFY(iconText.sizeHint().isValid());
    QVERIFY(iconText.sizeHint().width() >= textOnly.sizeHint().width());
    QVERIFY(iconText.minimumSizeHint().width() <= iconText.sizeHint().width());
    QVERIFY(iconText.minimumSizeHint().height() <= iconText.sizeHint().height());
}

void tst_ExtendedFabMaturity::materialMetadataIsExplicit()
{
    QtMaterial::QtMaterialExtendedFab button(QStringLiteral("Create"));
    QCOMPARE(button.materialComponent(), QStringLiteral("button"));
    QCOMPARE(button.materialVariant(), QStringLiteral("extended-fab"));
    QCOMPARE(button.materialRole(), QStringLiteral("action"));
}

QTEST_MAIN(tst_ExtendedFabMaturity)
#include "tst_extendedfab_maturity.moc"
