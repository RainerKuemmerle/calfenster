cmake_minimum_required(VERSION 3.16)

project(calfenster VERSION 0.0.1 DESCRIPTION "A dialog app displaying a calendar")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CALFENSTER_QTVERSION "6" CACHE STRING "Which Qt version to use. 6|5")
find_package(Qt${CALFENSTER_QTVERSION} COMPONENTS Core Widgets Network REQUIRED)

add_executable(calfenster
src/configuration.cpp
src/event_filter.cpp
src/app_server.cpp
src/clock_nanny.cpp
src/main.cpp)

target_compile_options(calfenster PRIVATE -Wall -Wextra -pedantic)
target_include_directories(calfenster PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(calfenster PRIVATE Qt${CALFENSTER_QTVERSION}::Core
  Qt${CALFENSTER_QTVERSION}::Widgets Qt${CALFENSTER_QTVERSION}::Network)
