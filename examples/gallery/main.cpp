#include <QApplication>
#include "gallerywindow.h"
int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    GalleryWindow window;
    window.resize(960, 720);
    window.show();
    return app.exec();
}
