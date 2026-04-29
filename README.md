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

| Layer            | Tool                                            | Why                                            |
|------------------|-------------------------------------------------|------------------------------------------------|
| Language         | C++20                                           | Native, OS-level access, what I want to learn  |
| Compiler         | MSVC 14.50 (Visual Studio 2026 Community)       | Required for Windows.Graphics.Capture and NVENC|
| UI framework     | Qt 6.10.3 (Widgets)                             | Tray icon, settings window, threading helpers  |
| Build system     | CMake 4.3                                       | Standard for modern cross-vendor C++           |
| Build tool       | Ninja                                           | Fast incremental builds                        |
| Package manager  | vcpkg (manifest mode)                           | For ffmpeg/opencv/tesseract    |
| Editor           | Visual Studio Code                              | Lightweight, Microsoft-supported with CMake Tools |
| Capture API      | Windows.Graphics.Capture (planned)              | Modern, low overhead, post-1809 Windows 10+    |
| Encoder          | NVENC via FFmpeg (planned)                      | Hardware accelerated, NVIDIA GPU on this machine |
| Computer vision  | OpenCV + Tesseract (planned)                    | Kill feed detection from captured frames       |

