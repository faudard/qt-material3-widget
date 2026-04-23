#include <QApplication>

#include "themestudiowindow.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    ThemeStudioWindow window;
    window.resize(1200, 780);
    window.show();

    return app.exec();
}
