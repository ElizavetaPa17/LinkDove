cmake_minimum_required(VERSION 3.5)

project(common_lib LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt REQUIRED COMPONENTS Sql Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Sql Widgets)
find_package(Qt6 REQUIRED COMPONENTS Multimedia)

set(LIB_FILES
    constants.h     constants.cpp
    IStreamConnection.h
    StatusInfo.h    StatusInfo.cpp
    tcpconnection.h tcpconnection.cpp
    UserInfo.h      UserInfo.cpp
    utility.h       utility.cpp
    logininfo.h     logininfo.cpp
    complaint.h     complaint.cpp
    shortstatusinfo.h shortstatusinfo.cpp
    imessage.h imessage.cpp
    individualmessage.h individualmessage.cpp
    imessagecontent.h imessagecontent.cpp
    textmessagecontent.h textmessagecontent.cpp
)

add_library(common_library ${LIB_FILES}
    imagemessagecontent.h imagemessagecontent.cpp
    channelinfo.h channelinfo.cpp
    channelmessage.h channelmessage.cpp
    chatinfo.h chatinfo.cpp
    groupmessage.h groupmessage.cpp
    notification.h notification.cpp

    audiomanager.h audiomanager.cpp
    audiomessagecontent.h audiomessagecontent.cpp
    audiomanagersingleton.h)
target_link_libraries(common_library PRIVATE Qt${QT_VERSION_MAJOR}::Sql Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Multimedia)
