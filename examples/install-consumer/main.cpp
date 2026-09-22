#include <QApplication>
#include <QLabel>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QLabel label(QStringLiteral("QtMaterial3 installed package consumer"));
    label.resize(420, 100);
    label.show();
    return app.exec();
}
