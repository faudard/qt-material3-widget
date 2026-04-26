#include <QtTest/QtTest>
#include <QByteArray>
#include <QColor>
#include <QElapsedTimer>
#include <QProcessEnvironment>

#include "qtmaterial/core/qtmaterialdensity.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemeserializer.h"

namespace {

bool enforceBudgets()
{
    return QProcessEnvironment::systemEnvironment()
        .value(QStringLiteral("QTMATERIAL3_ENFORCE_PERF_BUDGETS")) == QStringLiteral("1");
}

int budgetMs(const QString& variableName, int defaultValue)
{
    bool ok = false;
    const int value = QProcessEnvironment::systemEnvironment().value(variableName).toInt(&ok);
    return ok && value > 0 ? value : defaultValue;
}

void reportBudget(const char* label, qint64 elapsedMs, int budget)
{
    qInfo().noquote() << QString::fromLatin1("%1: %2 ms, budget %3 ms%4")
        .arg(QString::fromLatin1(label))
        .arg(elapsedMs)
        .arg(budget)
        .arg(enforceBudgets() ? QStringLiteral(" [enforced]") : QStringLiteral(" [reported only]"));
}

void maybeVerifyBudget(const char* label, qint64 elapsedMs, int budget)
{
    reportBudget(label, elapsedMs, budget);
    if (enforceBudgets()) {
        QVERIFY2(elapsedMs <= budget,
                 qPrintable(QString::fromLatin1("%1 exceeded budget: %2 ms > %3 ms")
                                .arg(QString::fromLatin1(label))
                                .arg(elapsedMs)
                                .arg(budget)));
    }
}

} // namespace

class tst_ThemePerformanceContracts : public QObject
{
    Q_OBJECT

private slots:
    void themeBuildSeedMatrixBudget()
    {
        QtMaterial::ThemeBuilder builder;
        const QVector<QColor> seeds {
            QColor(QStringLiteral("#6750A4")),
            QColor(QStringLiteral("#00639B")),
            QColor(QStringLiteral("#FF0000")),
            QColor(QStringLiteral("#0B8043")),
            QColor(QStringLiteral("#F9AB00"))
        };

        QElapsedTimer timer;
        timer.start();
        for (int iteration = 0; iteration < 250; ++iteration) {
            for (const QColor& seed : seeds) {
                auto light = builder.buildLightFromSeed(seed);
                auto dark = builder.buildDarkFromSeed(seed);
                Q_UNUSED(light);
                Q_UNUSED(dark);
            }
        }

        maybeVerifyBudget("themeBuildSeedMatrixBudget",
                          timer.elapsed(),
                          budgetMs(QStringLiteral("QTMATERIAL3_THEME_BUILD_BUDGET_MS"), 5000));
    }

    void jsonRoundTripBudget()
    {
        QtMaterial::ThemeBuilder builder;
        const auto theme = builder.buildDarkFromSeed(QColor(QStringLiteral("#6750A4")));
        const QByteArray json = QtMaterial::ThemeSerializer::toJson(theme, QJsonDocument::Compact);

        QElapsedTimer timer;
        timer.start();
        for (int iteration = 0; iteration < 1000; ++iteration) {
            bool ok = false;
            QString error;
            const auto parsed = QtMaterial::ThemeSerializer::fromJson(json, &ok, &error);
            QVERIFY2(ok, qPrintable(error));
            Q_UNUSED(parsed);
        }

        maybeVerifyBudget("jsonRoundTripBudget",
                          timer.elapsed(),
                          budgetMs(QStringLiteral("QTMATERIAL3_JSON_ROUNDTRIP_BUDGET_MS"), 5000));
    }

    void specFactoryHotLoopBudget()
    {
        QtMaterial::ThemeBuilder builder;
        const auto theme = builder.buildLightFromSeed(QColor(QStringLiteral("#00639B")));
        QtMaterial::SpecFactory factory;

        QElapsedTimer timer;
        timer.start();
        for (int iteration = 0; iteration < 10000; ++iteration) {
            auto filled = factory.filledButtonSpec(theme, QtMaterial::Density::Default);
            auto outlinedField = factory.outlinedTextFieldSpec(theme, QtMaterial::Density::Default);
            auto card = factory.cardSpec(theme);
            Q_UNUSED(filled);
            Q_UNUSED(outlinedField);
            Q_UNUSED(card);
        }

        maybeVerifyBudget("specFactoryHotLoopBudget",
                          timer.elapsed(),
                          budgetMs(QStringLiteral("QTMATERIAL3_SPEC_FACTORY_BUDGET_MS"), 5000));
    }
};

QTEST_MAIN(tst_ThemePerformanceContracts)
#include "tst_theme_performance_contracts.moc"
