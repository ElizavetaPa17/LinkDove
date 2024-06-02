cmake_minimum_required(VERSION 3.5)

project(LinkDoveServer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED system)
find_package(QT NAMES Qt6 Qt REQUIRED COMPONENTS Sql Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Sql Widgets)

set(PROJECT_SOURCES
    main.cpp
    linkdoveserver.h      linkdoveserver.cpp
    linkdovesqldatabase.h linkdovesqldatabase.cpp
)

set(COMMON_SOURCE_PATH ${CMAKE_SOURCE_DIR}/../common_source)

include_directories(${COMMON_SOURCE_PATH})
link_directories(${COMMON_SOURCE_PATH})
add_subdirectory(${COMMON_SOURCE_PATH} build)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(LinkDoveServer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        LinkDoveDatabase.h
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET LinkDoveServer APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(LinkDoveServer SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(LinkDoveServer
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(LinkDoveServer PRIVATE Qt${QT_VERSION_MAJOR}::Sql Qt${QT_VERSION_MAJOR}::Widgets
                                     common_library
                                     ${Boost_LIBRARIES})

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.LinkDoveServer)
endif()
set_target_properties(LinkDoveServer PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS LinkDoveServer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(LinkDoveServer)
endif()
