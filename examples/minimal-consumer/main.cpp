#include <QApplication>
#include <QWidget>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QWidget window;
    window.setWindowTitle(QStringLiteral("qt-material3-widgets minimal consumer"));
    window.resize(480, 320);
    window.show();
    return app.exec();
}
