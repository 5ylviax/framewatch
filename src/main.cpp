#include <QApplication>
#include <QLabel>
#include <thread>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QLabel label("Hello World!- testing phase 0 completed");
    label.show();
    return app.exec();
}