cmake_minimum_required(VERSION 3.24)
project(Framewatch LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.8 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()
qt_add_executable(Framewatch
    src/main.cpp
)
target_include_directories(Framewatch PRIVATE
    src
)
target_link_libraries(Framewatch PRIVATE Qt6::Widgets)