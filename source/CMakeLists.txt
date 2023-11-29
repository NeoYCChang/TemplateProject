cmake_minimum_required(VERSION 3.14)

project(TCP_Server LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Network)

add_executable(TCP_Server
  main.cpp
  server.h server.cpp
  tcpclientsocket.h tcpclientsocket.cpp
)
target_link_libraries(TCP_Server Qt${QT_VERSION_MAJOR}::Network)
target_link_libraries(TCP_Server Qt${QT_VERSION_MAJOR}::Core)

include(GNUInstallDirs)
install(TARGETS TCP_Server
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
