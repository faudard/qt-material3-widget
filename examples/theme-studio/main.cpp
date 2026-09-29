#include <QApplication>

#include "themestudiowindow.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    ThemeStudioWindow window;
    window.setMinimumSize(760, 500);
    window.resize(1100, 700);
    window.show();

    return app.exec();
}
