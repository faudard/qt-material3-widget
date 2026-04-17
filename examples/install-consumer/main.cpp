#include <QApplication>
#include <QLabel>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QLabel label(QStringLiteral("Installed package consumer placeholder"));
    label.resize(420, 100);
    label.show();
    return app.exec();
}
