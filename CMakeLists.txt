cmake_minimum_required(VERSION 3.16)

set(CALFENSTER_VERSION "0.3.0")

project(calfenster VERSION ${CALFENSTER_VERSION} DESCRIPTION "A dialog app displaying a calendar")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CALFENSTER_QTVERSION "6" CACHE STRING "Which Qt version to use. 6|5")
find_package(Qt${CALFENSTER_QTVERSION} COMPONENTS Core Widgets Network REQUIRED)

if (${CALFENSTER_QTVERSION} STREQUAL "6")
  find_package(LayerShellQt 6.1.0)
endif()
if (TARGET LayerShellQt::Interface)
  set(HAVE_LAYER_SHELL TRUE)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
${PROJECT_BINARY_DIR}/include/calfenster/config.h)

add_executable(calfenster
src/configuration.cpp
src/event_filter.cpp
src/app_server.cpp
src/clock_nanny.cpp
src/main.cpp)

target_compile_options(calfenster PRIVATE -Wall -Wextra -pedantic)
target_include_directories(calfenster PRIVATE
  ${PROJECT_BINARY_DIR}/include/
  ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(calfenster PRIVATE Qt${CALFENSTER_QTVERSION}::Core
  Qt${CALFENSTER_QTVERSION}::Widgets Qt${CALFENSTER_QTVERSION}::Network)
if (TARGET LayerShellQt::Interface)
  target_link_libraries(calfenster PRIVATE LayerShellQtInterface)
endif()

include(GNUInstallDirs)
install(TARGETS calfenster
    EXPORT calfenster
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
