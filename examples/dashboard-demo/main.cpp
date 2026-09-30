#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QFont>
#include <QFontDatabase>
#include <QPixmap>
#include <QSize>
#include <QTimer>

#include "dashboardwindow.h"

#include "qtmaterial/theme/qtmaterialthememanager.h"

namespace {

int pageIndex(const QString& page)
{
    const QString normalized = page.trimmed().toLower();
    if (normalized == QStringLiteral("analytics")) {
        return 1;
    }
    if (normalized == QStringLiteral("orders")) {
        return 2;
    }
    if (normalized == QStringLiteral("customers")) {
        return 3;
    }
    if (normalized == QStringLiteral("components")) {
        return 4;
    }
    if (normalized == QStringLiteral("profile")) {
        return 5;
    }
    if (normalized == QStringLiteral("pricing")) {
        return 6;
    }
    if (normalized == QStringLiteral("states")) {
        return 7;
    }
    if (normalized == QStringLiteral("settings")) {
        return 8;
    }
    return 0;
}

QSize parseSize(const QString& value)
{
    const QStringList parts = value.toLower().split(QLatin1Char('x'));
    if (parts.size() != 2) {
        return QSize(1440, 920);
    }

    bool widthOk = false;
    bool heightOk = false;
    const int width = parts.at(0).toInt(&widthOk);
    const int height = parts.at(1).toInt(&heightOk);
    if (!widthOk || !heightOk || width < 320 || height < 320) {
        return QSize(1440, 920);
    }
    return QSize(width, height);
}

} // namespace

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Qt Material 3 Dashboard Demo"));

    QCommandLineParser parser;
    parser.setApplicationDescription(
        QStringLiteral("Qt Material 3 responsive dashboard showcase"));
    parser.addHelpOption();

    const QCommandLineOption darkOption(
        QStringList{QStringLiteral("dark")},
        QStringLiteral("Start with the dark Material theme."));
    const QCommandLineOption rtlOption(
        QStringList{QStringLiteral("rtl")},
        QStringLiteral("Start the application in right-to-left layout."));
    const QCommandLineOption pageOption(
        QStringList{QStringLiteral("page")},
        QStringLiteral(
            "Open a page: dashboard, analytics, orders, customers, components, "
            "profile, pricing, states or settings."),
        QStringLiteral("name"),
        QStringLiteral("dashboard"));
    const QCommandLineOption screenshotOption(
        QStringList{QStringLiteral("screenshot")},
        QStringLiteral("Capture the shown window to a PNG file and exit."),
        QStringLiteral("file"));
    const QCommandLineOption sizeOption(
        QStringList{QStringLiteral("size")},
        QStringLiteral("Window size used by the showcase, for example 1440x920."),
        QStringLiteral("widthxheight"),
        QStringLiteral("1440x920"));

    parser.addOption(darkOption);
    parser.addOption(rtlOption);
    parser.addOption(pageOption);
    parser.addOption(screenshotOption);
    parser.addOption(sizeOption);
    parser.process(app);

    QFont font = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
    font.setPointSize(10);
    app.setFont(font);

    if (parser.isSet(rtlOption)) {
        app.setLayoutDirection(Qt::RightToLeft);
    }

    if (parser.isSet(darkOption)) {
        auto options = QtMaterial::ThemeManager::instance().options();
        options.mode = QtMaterial::ThemeMode::Dark;
        options.preference = QtMaterial::ThemePreference::Dark;
        QtMaterial::ThemeManager::instance().setThemeOptions(options);
    }

    DashboardWindow window;
    window.resize(parseSize(parser.value(sizeOption)));
    window.showDemoPage(pageIndex(parser.value(pageOption)));
    window.show();

    if (parser.isSet(screenshotOption)) {
        const QString screenshotPath = parser.value(screenshotOption);
        QTimer::singleShot(350, &app, [&app, &window, screenshotPath]() {
            const bool saved = window.grab().save(screenshotPath, "PNG");
            app.exit(saved ? 0 : 2);
        });
    }

    return app.exec();
}
