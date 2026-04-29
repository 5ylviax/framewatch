#include <QApplication>
#include <QLabel>
#include <QApplication>    
#include <QThread>          
#include <QSystemTrayIcon>  
#include <QMenu>            
#include <windows.h>       
#include <tlhelp32.h>

// Windows.Graphics.Capture (modern WinRT capture API)
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>

// Direct3D 11 (the GPU API WGC uses)
#include <d3d11.h>
#include <dxgi1_2.h>


int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QLabel label("Hello World!- testing phase 0 completed");
    label.show();
    return app.exec();
}