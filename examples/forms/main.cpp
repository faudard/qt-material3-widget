#include <QApplication>
#include "formgallerywindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    FormGalleryWindow window;
    window.resize(1100, 760);
    window.show();
    return app.exec();
}
