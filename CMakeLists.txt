cmake_minimum_required(VERSION 3.16)

project(calfenster VERSION 0.0.1 DESCRIPTION "A dialog app displaying a calendar")

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Xml OpenGL Gui Widgets QUIET)

add_executable(calfenster
    src/main.cpp)

target_compile_options(calfenster PRIVATE -Wall -Wextra -pedantic -Werror)
target_include_directories(calfenster PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(calfenster PRIVATE Qt5::Widgets)
