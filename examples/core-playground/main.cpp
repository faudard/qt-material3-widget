#include <QApplication>
#include "testcontrolwidget.h"
int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    TestControlWidget widget;
    widget.resize(280, 72);
    widget.show();
    return app.exec();
}
