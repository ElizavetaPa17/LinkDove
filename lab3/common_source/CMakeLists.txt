cmake_minimum_required(VERSION 3.5)

project(common_lib LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt REQUIRED COMPONENTS Sql Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Sql Widgets)

set(LIB_FILES
    IStreamConnection.h
    StatusInfo.cpp
    StatusInfo.h
    tcpconnection.cpp
    tcpconnection.h
    UserInfo.cpp
    UserInfo.h
    utility.cpp
    utility.h)

add_library(common_library ${LIB_FILES})
target_link_libraries(common_library PRIVATE Qt${QT_VERSION_MAJOR}::Sql Qt${QT_VERSION_MAJOR}::Widgets)
