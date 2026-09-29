#include <QApplication>
#include <QFont>
#include <QFontDatabase>

#include "dashboardwindow.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Qt Material 3 Dashboard Demo"));

    QFont font = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
    font.setPointSize(10);
    app.setFont(font);

    DashboardWindow window;
    window.resize(1440, 920);
    window.show();
    return app.exec();
}
