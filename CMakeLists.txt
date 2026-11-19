cmake_minimum_required(VERSION 3.24)
project(Framewatch LANGUAGES CXX)
include(GNUInstallDirs)   # after project(): it needs the compiler and architecture to pick install paths

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.8 REQUIRED COMPONENTS Widgets Multimedia MultimediaWidgets)

qt_standard_project_setup()
qt_add_executable(Framewatch
    src/main.cpp
    src/clips/clip_browser.cpp
    src/clips/clip_browser.h
    src/clips/clip_status.cpp
    src/clips/clip_status.h
    src/clips/decode_errors.cpp
    src/clips/decode_errors.h
    src/clips/video_info.cpp
    src/clips/video_info.h
)
target_include_directories(Framewatch PRIVATE
    src
)
target_link_libraries(Framewatch PRIVATE
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::MultimediaWidgets
)

# ---- Packaging: `cmake --install` builds a folder you can zip and share ----

# Release builds open as a normal window app, without a black console window behind it.
# Debug builds keep the console so FFmpeg's red error lines still show while you work.
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_target_properties(Framewatch PROPERTIES WIN32_EXECUTABLE ON)
endif()

install(TARGETS Framewatch RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Runs windeployqt: copies the Qt DLLs, plugins and Qt's FFmpeg DLLs next to the exe
qt_generate_deploy_app_script(
    TARGET Framewatch
    OUTPUT_SCRIPT deploy_script
    NO_TRANSLATIONS
    NO_UNSUPPORTED_PLATFORM_ERROR
    # Skip vc_redist.x64.exe; the runtime DLLs are copied directly below
    DEPLOY_TOOL_OPTIONS --no-compiler-runtime
)
install(SCRIPT ${deploy_script})

# Copies the Microsoft C++ runtime DLLs, so people don't need the VC++ Redistributable
set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ${CMAKE_INSTALL_BINDIR})
include(InstallRequiredSystemLibraries)