# Framewatch

Review and clean up your game clips fast. Framewatch finds every clip in your
Videos folder so you can flip through them with the arrow keys and delete the
ones you don't want, without opening and closing each video.

In the future, Framewatch will also clip your games for you: a rolling replay
buffer with automatic highlight detection, starting with Apex Legends and more
games after. Zero ads. 

![Framewatch showing the clip list with resolution filters next to a 4K Apex Legends clip playing](image/FrameWatch-desktop-application.png)

## Run
From the terminal:
    ```bash 
    C:\dev\framewatch\build\debug\Framewatch.exe
    ```

## Tech stack

| Layer            | Tool                                      |
|------------------|-------------------------------------------|
| Language         | C++20                                     |
| Compiler         | MSVC               |
| UI framework     | Qt 6.8.3 (Widgets, Multimedia)            |
| Build system     | CMake 4.3                                 |
| Package manager  | vcpkg                                     |
| Capture API      | Windows.Graphics.Capture                  |
| Encoder          | NVENC via FFmpeg                          |
| Computer vision  | OpenCV + Tesseract                        |

