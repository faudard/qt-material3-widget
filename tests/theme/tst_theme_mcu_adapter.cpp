#include <QtTest/QtTest>

#include <array>

#include "qtmaterial/theme/qtmaterialcolorbackend.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_ThemeMcuAdapter : public QObject {
    Q_OBJECT

private slots:
    void mcuStatusReflectsCompiledAdapter();
    void mcuGeneratesCompleteScheme();
    void mcuContrastChangesResolvedRoles();
    void mcuLightDarkParity();
    void directSeedSchemeUsesMcuWhenAvailable();
};

static std::array<ColorRole, 49> requiredColorRoles()
{
    return {
        ColorRole::Primary,
        ColorRole::OnPrimary,
        ColorRole::PrimaryContainer,
        ColorRole::OnPrimaryContainer,
        ColorRole::PrimaryFixed,
        ColorRole::PrimaryFixedDim,
        ColorRole::OnPrimaryFixed,
        ColorRole::OnPrimaryFixedVariant,
        ColorRole::Secondary,
        ColorRole::OnSecondary,
        ColorRole::SecondaryContainer,
        ColorRole::OnSecondaryContainer,
        ColorRole::SecondaryFixed,
        ColorRole::SecondaryFixedDim,
        ColorRole::OnSecondaryFixed,
        ColorRole::OnSecondaryFixedVariant,
        ColorRole::Tertiary,
        ColorRole::OnTertiary,
        ColorRole::TertiaryContainer,
        ColorRole::OnTertiaryContainer,
        ColorRole::TertiaryFixed,
        ColorRole::TertiaryFixedDim,
        ColorRole::OnTertiaryFixed,
        ColorRole::OnTertiaryFixedVariant,
        ColorRole::Error,
        ColorRole::OnError,
        ColorRole::ErrorContainer,
        ColorRole::OnErrorContainer,
        ColorRole::Background,
        ColorRole::OnBackground,
        ColorRole::Surface,
        ColorRole::OnSurface,
        ColorRole::SurfaceDim,
        ColorRole::SurfaceBright,
        ColorRole::SurfaceContainerLowest,
        ColorRole::SurfaceContainerLow,
        ColorRole::SurfaceContainer,
        ColorRole::SurfaceContainerHigh,
        ColorRole::SurfaceContainerHighest,
        ColorRole::SurfaceVariant,
        ColorRole::OnSurfaceVariant,
        ColorRole::SurfaceTint,
        ColorRole::Outline,
        ColorRole::OutlineVariant,
        ColorRole::InverseSurface,
        ColorRole::InverseOnSurface,
        ColorRole::InversePrimary,
        ColorRole::Shadow,
        ColorRole::Scrim
    };
}

static void skipIfNoMcu()
{
    if (!ThemeBuilder::isMaterialColorUtilitiesAvailable()) {
        QSKIP("Material Color Utilities backend is not compiled into this build.");
    }
}

static ThemeOptions mcuOptions(ThemeMode mode, ContrastMode contrast = ContrastMode::Standard)
{
    ThemeOptions options;
    options.sourceColor = QColor("#6750A4");
    options.mode = mode;
    options.contrast = contrast;
    options.useMaterialColorUtilities = true;
    return options;
}

void tst_ThemeMcuAdapter::mcuStatusReflectsCompiledAdapter()
{
    ThemeBuilder builder;
    ThemeOptions options = mcuOptions(ThemeMode::Light);
    const ThemeColorBackendStatus status = builder.colorBackendStatus(options);

#if defined(QTMATERIAL3_HAS_MCU) && QTMATERIAL3_HAS_MCU
    QVERIFY(ThemeBuilder::isMaterialColorUtilitiesAvailable());
    QCOMPARE(status.requestedBackend, ThemeColorBackend::MaterialColorUtilities);
    QCOMPARE(status.effectiveBackend, ThemeColorBackend::MaterialColorUtilities);
    QVERIFY(status.materialColorUtilitiesCompiledIn);
    QVERIFY(!status.fallbackUsed);
#else
    QVERIFY(!ThemeBuilder::isMaterialColorUtilitiesAvailable());
    QCOMPARE(status.requestedBackend, ThemeColorBackend::MaterialColorUtilities);
    QCOMPARE(status.effectiveBackend, ThemeColorBackend::Fallback);
    QVERIFY(!status.materialColorUtilitiesCompiledIn);
    QVERIFY(status.fallbackUsed);
#endif
}

void tst_ThemeMcuAdapter::mcuGeneratesCompleteScheme()
{
    skipIfNoMcu();

    ThemeBuilder builder;
    const Theme theme = builder.build(mcuOptions(ThemeMode::Light));

    for (ColorRole role : requiredColorRoles()) {
        QVERIFY2(theme.colorScheme().contains(role), "MCU scheme did not resolve a required ColorRole.");
        QVERIFY(theme.colorScheme().color(role).isValid());
    }
}

void tst_ThemeMcuAdapter::mcuContrastChangesResolvedRoles()
{
    skipIfNoMcu();

    ThemeBuilder builder;
    const Theme standard = builder.build(mcuOptions(ThemeMode::Light, ContrastMode::Standard));
    const Theme high = builder.build(mcuOptions(ThemeMode::Light, ContrastMode::High));

    bool anyDifference = false;
    for (ColorRole role : requiredColorRoles()) {
        if (standard.colorScheme().color(role) != high.colorScheme().color(role)) {
            anyDifference = true;
            break;
        }
    }

    QVERIFY2(anyDifference, "MCU high-contrast generation should differ from standard contrast for at least one resolved role.");
}

void tst_ThemeMcuAdapter::mcuLightDarkParity()
{
    skipIfNoMcu();

    ThemeBuilder builder;
    const Theme light = builder.build(mcuOptions(ThemeMode::Light));
    const Theme dark = builder.build(mcuOptions(ThemeMode::Dark));

    for (ColorRole role : requiredColorRoles()) {
        QVERIFY(light.colorScheme().contains(role));
        QVERIFY(dark.colorScheme().contains(role));
    }

    QVERIFY(light.colorScheme().color(ColorRole::Surface) != dark.colorScheme().color(ColorRole::Surface));
}

void tst_ThemeMcuAdapter::directSeedSchemeUsesMcuWhenAvailable()
{
    skipIfNoMcu();

    ThemeBuilder builder;
    const ColorScheme direct = builder.buildLightSchemeFromSeed(QColor("#6750A4"));
    const Theme theme = builder.build(mcuOptions(ThemeMode::Light));

    QCOMPARE(direct.color(ColorRole::Primary), theme.colorScheme().color(ColorRole::Primary));
    QCOMPARE(direct.color(ColorRole::Surface), theme.colorScheme().color(ColorRole::Surface));
    QCOMPARE(direct.color(ColorRole::SurfaceTint), theme.colorScheme().color(ColorRole::SurfaceTint));
}

QTEST_MAIN(tst_ThemeMcuAdapter)
#include "tst_theme_mcu_adapter.moc"
