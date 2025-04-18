cmake_minimum_required(VERSION 3.16)

project(AudioPlayer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(THIRD_PARTY_PATH ${CMAKE_SOURCE_DIR}/thirdparty)
set(QHOTKEY_LIB_PATH ${THIRD_PARTY_PATH}/QHotkey)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia)

set(PROJECT_SOURCES
        main.cpp
        MainWindow.cpp
        MainWindow.h
        MainWindow.ui
        Widgets/MediaEntry.cpp
        Widgets/MediaEntry.h
        Widgets/MediaEntry.ui
        Widgets/PlayListWidget.cpp
        Widgets/PlayListWidget.h
        Widgets/PlayListWidget.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(AudioPlayer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        resources.qrc
        app_icon.rc
        Tools/QMediaPlayList.h Tools/QMediaPlayList.cpp
        Widgets/QMaskWidget.h Widgets/QMaskWidget.cpp
        Widgets/QSlidePanel.h Widgets/QSlidePanel.cpp
        Tools/MediaMetadataExtractor.h Tools/MediaMetadataExtractor.cpp
        Tools/AlbumManager.h Tools/AlbumManager.cpp
        Widgets/AlbumLoadDialog.h Widgets/AlbumLoadDialog.cpp Widgets/AlbumLoadDialog.ui
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET AudioPlayer APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(AudioPlayer SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(AudioPlayer
            ${PROJECT_SOURCES}

        )
    endif()
endif()

add_subdirectory(${THIRD_PARTY_PATH}/QHotkey EXCLUDE_FROM_ALL)

target_link_libraries(AudioPlayer PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Multimedia
    QHotkey)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.AudioPlayer)
endif()
set_target_properties(AudioPlayer PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS AudioPlayer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(AudioPlayer)
endif()
