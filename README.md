# Framewatch
Open-source game clip recorder for Windows. Rolling buffer, automatic highlight detection, zero ads, zero cloud. Apex Legends supported, more games planned.

## Run

After building, launch the exe any of these ways:

- **F5** in VS Code (runs with debugger attached)
- Double-click `build\debug\Framewatch.exe` in File Explorer
- From the terminal:
    ```bash 
    C:\dev\framewatch\build\debug\Framewatch.exe
    ```

## Tech stack

| Layer            | Tool                                      |
|------------------|-------------------------------------------|
| Language         | C++20                                     |
| Compiler         | MSVC (Visual Studio 2026)                 |
| UI framework     | Qt 6.10.3 (Widgets)                       |
| Build system     | CMake 4.3                                 |
| Package manager  | vcpkg                                     |
| Capture API      | Windows.Graphics.Capture                  |
| Encoder          | NVENC via FFmpeg                          |
| Computer vision  | OpenCV + Tesseract                        |

