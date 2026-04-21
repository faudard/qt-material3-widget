#include <QApplication>

#include "themepreviewwindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    ThemePreviewWindow window;
    window.resize(960, 760);
    window.show();

    return app.exec();
}
