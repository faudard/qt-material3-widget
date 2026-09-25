#include <QtTest/QtTest>

#include <type_traits>

#include "qtmaterial/theme/qtmaterialthemebuilder.h"

class ThemeIdentityTest : public QObject
{
    Q_OBJECT

private slots:
    void themeIsARegularValue();
    void copyIsStructurallyEqual();
    void colorMutationChangesIdentity();
    void typographyMutationChangesIdentity();
    void componentOverrideMutationChangesIdentity();
};

void ThemeIdentityTest::themeIsARegularValue()
{
    static_assert(std::is_copy_constructible<QtMaterial::Theme>::value,
                  "Theme must be copy constructible");
    static_assert(std::is_copy_assignable<QtMaterial::Theme>::value,
                  "Theme must be copy assignable");
    static_assert(std::is_move_constructible<QtMaterial::Theme>::value,
                  "Theme must be move constructible");
    static_assert(std::is_move_assignable<QtMaterial::Theme>::value,
                  "Theme must be move assignable");
    QVERIFY(true);
}

void ThemeIdentityTest::copyIsStructurallyEqual()
{
    QtMaterial::ThemeBuilder builder;
    const QtMaterial::Theme original =
        builder.buildLightFromSeed(QColor(QStringLiteral("#6750A4")));
    const QtMaterial::Theme copy = original;

    QVERIFY(original == copy);
    QVERIFY(!(original != copy));
}

void ThemeIdentityTest::colorMutationChangesIdentity()
{
    QtMaterial::ThemeBuilder builder;
    const QtMaterial::Theme original =
        builder.buildLightFromSeed(QColor(QStringLiteral("#6750A4")));
    QtMaterial::Theme changed = original;

    changed.colorScheme().setColor(
        QtMaterial::ColorRole::Primary,
        QColor(QStringLiteral("#123456"))
    );

    QVERIFY(original != changed);
}

void ThemeIdentityTest::typographyMutationChangesIdentity()
{
    QtMaterial::ThemeBuilder builder;
    const QtMaterial::Theme original =
        builder.buildLightFromSeed(QColor(QStringLiteral("#6750A4")));
    QtMaterial::Theme changed = original;

    QtMaterial::TypographyStyle style =
        changed.typography().style(QtMaterial::TypeRole::BodyMedium);
    style.letterSpacing += 0.25;
    changed.typography().setStyle(QtMaterial::TypeRole::BodyMedium, style);

    QVERIFY(original != changed);
}

void ThemeIdentityTest::componentOverrideMutationChangesIdentity()
{
    QtMaterial::ThemeBuilder builder;
    const QtMaterial::Theme original =
        builder.buildLightFromSeed(QColor(QStringLiteral("#6750A4")));
    QtMaterial::Theme changed = original;

    QtMaterial::ComponentTokenOverride overrideTokens;
    overrideTokens.colors.insert(
        QtMaterial::ColorRole::Primary,
        QColor(QStringLiteral("#FF0000"))
    );
    changed.componentOverrides().setOverride(
        QtMaterial::ComponentId::Button,
        overrideTokens
    );

    QVERIFY(original != changed);
}

QTEST_MAIN(ThemeIdentityTest)
#include "tst_themeidentity.moc"
