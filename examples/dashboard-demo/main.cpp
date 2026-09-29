#include <QApplication>

#include "dashboardwindow.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Qt Material 3 Dashboard Demo"));

    DashboardWindow window;
    window.resize(1380, 900);
    window.show();
    return app.exec();
}
