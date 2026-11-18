#include <QApplication>
#include <QMainWindow>
#include <QTabWidget>
#include <windows.h>

#include "clips/clip_browser.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("Framewatch");   // names the AppData\Roaming\Framewatch folder

    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    QMainWindow window;
    window.setWindowTitle("Framewatch");
    window.resize(1280, 720);

    auto* tabs = new QTabWidget;
    tabs->addTab(new ClipBrowser, "Clips");
    // Later: tabs->addTab(new Recorder, "Record");
    window.setCentralWidget(tabs);

    window.show();
    return app.exec();
}
