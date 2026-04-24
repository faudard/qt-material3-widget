#include <QtTest/QtTest>

#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include "qtmaterial/theme/qtmaterialtheme.h"
#include "qtmaterial/theme/qtmaterialxmlthemeadapter.h"

using namespace QtMaterial;

class tst_XmlThemeAdapter : public QObject
{
    Q_OBJECT

private slots:
    void import_minimalXml_ok();
    void import_missingRequiredColor_fails();
    void roundTrip_preservesMappedRoles();
};

static QByteArray minimalXml()
{
    return R"XML(<?xml version="1.0" encoding="UTF-8"?>
<resources>
  <color name="primaryColor">#00E5FF</color>
  <color name="primaryLightColor">#6EFFFF</color>
  <color name="secondaryColor">#FAFAFA</color>
  <color name="secondaryLightColor">#FFFFFF</color>
  <color name="secondaryDarkColor">#E0E0E0</color>
  <color name="primaryTextColor">#000000</color>
  <color name="secondaryTextColor">#212121</color>
</resources>)XML";
}

void tst_XmlThemeAdapter::import_minimalXml_ok()
{
    bool ok = false;
    QString error;
    const Theme theme = XmlThemeAdapter::fromQtMaterialXml(minimalXml(), &ok, &error);

    QVERIFY2(ok, qPrintable(error));
    QCOMPARE(theme.colorScheme().color(ColorRole::Primary), QColor(QStringLiteral("#00E5FF")));
    QCOMPARE(theme.colorScheme().color(ColorRole::OnPrimary), QColor(QStringLiteral("#000000")));
    QCOMPARE(theme.colorScheme().color(ColorRole::Surface), QColor(QStringLiteral("#FAFAFA")));
    QCOMPARE(theme.colorScheme().color(ColorRole::OnSurface), QColor(QStringLiteral("#212121")));
}

void tst_XmlThemeAdapter::import_missingRequiredColor_fails()
{
    const QByteArray xml = R"XML(<resources><color name="primaryColor">#00E5FF</color></resources>)XML";
    bool ok = true;
    QString error;
    const Theme theme = XmlThemeAdapter::fromQtMaterialXml(xml, &ok, &error);
    Q_UNUSED(theme);

    QVERIFY(!ok);
    QVERIFY(!error.isEmpty());
}

void tst_XmlThemeAdapter::roundTrip_preservesMappedRoles()
{
    bool ok = false;
    QString error;
    const Theme imported = XmlThemeAdapter::fromQtMaterialXml(minimalXml(), &ok, &error);
    QVERIFY2(ok, qPrintable(error));

    const QByteArray exported = XmlThemeAdapter::toQtMaterialXml(imported);
    const Theme restored = XmlThemeAdapter::fromQtMaterialXml(exported, &ok, &error);
    QVERIFY2(ok, qPrintable(error));

    QCOMPARE(restored.colorScheme().color(ColorRole::Primary),
             imported.colorScheme().color(ColorRole::Primary));
    QCOMPARE(restored.colorScheme().color(ColorRole::OnPrimary),
             imported.colorScheme().color(ColorRole::OnPrimary));
    QCOMPARE(restored.colorScheme().color(ColorRole::Surface),
             imported.colorScheme().color(ColorRole::Surface));
    QCOMPARE(restored.colorScheme().color(ColorRole::OnSurface),
             imported.colorScheme().color(ColorRole::OnSurface));
}

QTEST_MAIN(tst_XmlThemeAdapter)
#include "tst_xmlthemeadapter.moc"
