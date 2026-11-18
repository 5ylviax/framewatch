cmake_minimum_required(VERSION 3.24)
project(Framewatch LANGUAGES CXX)

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