cmake_minimum_required(VERSION 3.5)

project(LinkDove VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Multimedia)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        client.h client.cpp client.qrc
        clientsingleton.h
)

set(COMMON_SOURCE_PATH ${CMAKE_SOURCE_DIR}/../common_source)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${COMMON_SOURCE_PATH})

link_directories(${COMMON_SOURCE_PATH})
add_subdirectory(${COMMON_SOURCE_PATH} build)

# Создаем директорию, в которой будут хранится картинки из чата с двумя собеседниками
add_custom_target(ind-images-directory ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ./media/ind_images)
# Создаем директорию, в которой будут хранится картинки из групп
add_custom_target(group-images-directory ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ./media/group_images)
# Создаем директорию, в которой будут хранится картинки из каналов
add_custom_target(channel-images-directory ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ./media/channel_images)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(LinkDove
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        welcomewidget.h welcomewidget.cpp
        transparentlabel.h transparentlabel.cpp
        loginwidget.h loginwidget.cpp loginwidget.ui
        clickablelabel.h clickablelabel.cpp
        registrationwidget.h registrationwidget.cpp registrationwidget.ui
        mainwidget.h mainwidget.cpp mainwidget.ui
        profilewidget.h profilewidget.cpp profilewidget.ui
        settingwidget.h settingwidget.cpp settingwidget.ui
        infodialog.h infodialog.cpp infodialog.ui
        editprofilewidget.h editprofilewidget.cpp editprofilewidget.ui
        editprofilewidget.h editprofilewidget.cpp editprofilewidget.ui
        complaintcard.h complaintcard.cpp complaintcard.ui
        complaintslistdialog.h complaintslistdialog.cpp
        userslist.h userslist.cpp
        userslist.h userslist.cpp userslist.ui
        usercard.h usercard.cpp usercard.ui
        chatwidget.h chatwidget.cpp chatwidget.ui
        messagecard.h messagecard.cpp messagecard.ui
        bandialog.h bandialog.cpp bandialog.ui
        interlocutorprofiledialog.h interlocutorprofiledialog.cpp interlocutorprofiledialog.ui
        channelwidget.h channelwidget.cpp channelwidget.ui
        channellist.h channellist.cpp channellist.ui
        channelcard.h channelcard.cpp channelcard.ui
        imagedisplayingdialog.h imagedisplayingdialog.cpp imagedisplayingdialog.ui
        chatlist.h chatlist.cpp chatlist.ui
        chatcard.h chatcard.cpp chatcard.ui
        groupwidget.h groupwidget.cpp groupwidget.ui
        agreedialog.h agreedialog.cpp agreedialog.ui
        listlabeldialog.h listlabeldialog.cpp listlabeldialog.ui
        typestringdialog.h typestringdialog.cpp typestringdialog.ui
        notificationlistdialog.h notificationlistdialog.cpp notificationlistdialog.ui
        notificationcard.h notificationcard.cpp notificationcard.ui
        baninterlocutordialog.h baninterlocutordialog.cpp baninterlocutordialog.ui
        createbroadchatdialog.h createbroadchatdialog.cpp createbroadchatdialog.ui
        requestlistdialog.h requestlistdialog.cpp
        requestcard.h requestcard.cpp requestcard.ui
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET LinkDove APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(LinkDove SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(LinkDove
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(LinkDove PRIVATE Qt6::Sql Qt6::Multimedia)
target_link_libraries(LinkDove PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
                               common_library)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.LinkDove)
endif()
set_target_properties(LinkDove PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS LinkDove
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(LinkDove)
endif()
