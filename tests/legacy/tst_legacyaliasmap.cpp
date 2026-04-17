#include <QtTest/QtTest>
#include "qtmaterial/theme/qtmateriallegacyaliasmap.h"

class tst_LegacyAliasMap : public QObject
{
    Q_OBJECT
private slots:
    void resolvesKnownKeys();
    void rejectsUnknownKeys();
};

void tst_LegacyAliasMap::resolvesKnownKeys() {
    const auto primary = QtMaterial::LegacyAliasMap::colorRoleForKey(QStringLiteral("primary1"));
    QVERIFY(primary.has_value());
    QCOMPARE(*primary, QtMaterial::ColorRole::Primary);

    const auto text = QtMaterial::LegacyAliasMap::colorRoleForKey(QStringLiteral("text"));
    QVERIFY(text.has_value());
    QCOMPARE(*text, QtMaterial::ColorRole::OnSurface);
}

void tst_LegacyAliasMap::rejectsUnknownKeys() {
    const auto role = QtMaterial::LegacyAliasMap::colorRoleForKey(QStringLiteral("banana"));
    QVERIFY(!role.has_value());
}

QTEST_MAIN(tst_LegacyAliasMap)
#include "tst_legacyaliasmap.moc"
