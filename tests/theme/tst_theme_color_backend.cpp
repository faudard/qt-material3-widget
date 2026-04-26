#include <QtTest/QtTest>

#include "qtmaterial/theme/qtmaterialcolorbackend.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_ThemeColorBackend : public QObject {
    Q_OBJECT

private slots:
    void compiledBackendIsObservable();
    void fallbackIsSelectedWhenMcuIsNotCompiled();
    void explicitFallbackRequestStaysFallback();
    void contrastModeMappingMatchesMcuLevels();
};

void tst_ThemeColorBackend::compiledBackendIsObservable()
{
#if defined(QTMATERIAL3_HAS_MCU) && QTMATERIAL3_HAS_MCU
    QCOMPARE(compiledThemeColorBackend(), ThemeColorBackend::MaterialColorUtilities);
    QVERIFY(isMaterialColorUtilitiesCompiledIn());
#else
    QCOMPARE(compiledThemeColorBackend(), ThemeColorBackend::Fallback);
    QVERIFY(!isMaterialColorUtilitiesCompiledIn());
#endif
}

void tst_ThemeColorBackend::fallbackIsSelectedWhenMcuIsNotCompiled()
{
    ThemeOptions options;
    options.useMaterialColorUtilities = true;

    const ThemeColorBackendStatus status = resolveThemeColorBackend(options);

#if defined(QTMATERIAL3_HAS_MCU) && QTMATERIAL3_HAS_MCU
    QCOMPARE(status.effectiveBackend, ThemeColorBackend::MaterialColorUtilities);
    QVERIFY(!status.fallbackUsed);
#else
    QCOMPARE(status.requestedBackend, ThemeColorBackend::MaterialColorUtilities);
    QCOMPARE(status.effectiveBackend, ThemeColorBackend::Fallback);
    QVERIFY(status.fallbackUsed);
    QVERIFY(status.diagnostic.contains(QStringLiteral("fallback"), Qt::CaseInsensitive));
#endif
}

void tst_ThemeColorBackend::explicitFallbackRequestStaysFallback()
{
    ThemeOptions options;
    options.useMaterialColorUtilities = false;

    const ThemeBuilder builder;
    const ThemeColorBackendStatus status = builder.colorBackendStatus(options);

    QCOMPARE(status.requestedBackend, ThemeColorBackend::Fallback);
    QCOMPARE(status.effectiveBackend, ThemeColorBackend::Fallback);
    QVERIFY(status.fallbackUsed);
}

void tst_ThemeColorBackend::contrastModeMappingMatchesMcuLevels()
{
    QCOMPARE(contrastModeToMaterialColorUtilitiesLevel(ContrastMode::Standard), 0.0);
    QCOMPARE(contrastModeToMaterialColorUtilitiesLevel(ContrastMode::Medium), 0.5);
    QCOMPARE(contrastModeToMaterialColorUtilitiesLevel(ContrastMode::High), 1.0);
}

QTEST_MAIN(tst_ThemeColorBackend)
#include "tst_theme_color_backend.moc"
