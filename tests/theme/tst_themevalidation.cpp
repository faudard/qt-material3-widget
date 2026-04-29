#include <QtTest/QtTest>

#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemevalidation.h"

class tst_ThemeValidation : public QObject {
    Q_OBJECT

private slots:
    void builderProducesValidLightTheme()
    {
        QtMaterial::ThemeBuilder builder;
        QtMaterial::ThemeOptions options;
        options.sourceColor = QColor(QStringLiteral("#6750A4"));
        options.mode = QtMaterial::ThemeMode::Light;
        options.preference = QtMaterial::ThemePreference::Light;
        options.backendPolicy = QtMaterial::ColorBackendPolicy::ForceFallback;

        const QtMaterial::Theme theme = builder.build(options);
        const QtMaterial::ThemeValidationResult result = QtMaterial::validateTheme(theme);

        QVERIFY2(result.isValid(), qPrintable(result.errors().join(QStringLiteral("\n"))));
        QVERIFY(theme.colorScheme().isComplete());
    }

    void missingColorRolesAreErrors()
    {
        QtMaterial::ColorScheme scheme;
        QtMaterial::ThemeValidationPolicy policy;
        policy.validateContrast = false;

        const QtMaterial::ThemeValidationResult result = QtMaterial::validateColorScheme(scheme, policy);

        QVERIFY(!result.isValid());
        QVERIFY(!result.errors().isEmpty());
    }
};

QTEST_MAIN(tst_ThemeValidation)
#include "tst_themevalidation.moc"
