cmake_minimum_required(VERSION 3.16)

project(ra2snes VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools WebSockets Quick Qml QuickWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools WebSockets Quick Qml QuickWidgets)

include_directories(${CMAKE_SOURCE_DIR}/rcheevos/include)
include_directories(${CMAKE_SOURCE_DIR}/rcheevos/src)
include_directories(${CMAKE_SOURCE_DIR}/rcheevos/src/rapi)
include_directories(${CMAKE_SOURCE_DIR}/rcheevos/src/rcheevos)
include_directories(${CMAKE_SOURCE_DIR}/rcheevos/src/rhash)
include_directories(${CMAKE_SOURCE_DIR}/rcheevos/src/rurl)

set(TS_FILES ra2snes_en_US.ts)

file(GLOB RCHEEVOS_SOURCES ${CMAKE_SOURCE_DIR}/rcheevos/src/*)
file(GLOB RCHEEVOS_INCLUDE ${CMAKE_SOURCE_DIR}/rcheevos/include/*)
file(GLOB RCHEEVOS_SOURCES_RAPI ${CMAKE_SOURCE_DIR}/rcheevos/src/rapi/*)
file(GLOB RCHEEVOS_SOURCES_RCHEEVOS ${CMAKE_SOURCE_DIR}/rcheevos/src/rcheevos/*)
file(GLOB RCHEEVOS_SOURCES_RHASH ${CMAKE_SOURCE_DIR}/rcheevos/src/rhash/*)
file(GLOB RCHEEVOS_SOURCES_RURL ${CMAKE_SOURCE_DIR}/rcheevos/src/rurl/*)
file(GLOB LUA_SOURCES ${CMAKE_SOURCE_DIR}/rcheevos/test/lua/src/*)
file(GLOB UI_FILES ${CMAKE_SOURCE_DIR}/ui/*)

set(PROJECT_SOURCES
        main.cpp
        ra2snes.cpp
        ra2snes.h
        ra2snes.ui
        ${TS_FILES}
        usb2snes.cpp
        usb2snes.h
        raclient.cpp
        raclient.h
        memoryreader.cpp
        memoryreader.h
        rastructs.h
        ${RCHEEVOS_SOURCES}
        ${RCHEEVOS_INCLUDE}
        ${RCHEEVOS_SOURCES_RCHEEVOS}
        ${RCHEEVOS_SOURCES_RAPI}
        ${RCHEEVOS_SOURCES_RHASH}
        ${RCHEEVOS_SOURCES_RURL}
        ${LUA_SOURCES}
        ${UI_FILES}
        ui.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ra2snes
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(ra2snes SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(ra2snes
            ${PROJECT_SOURCES}
        )
    endif()
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(ra2snes PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(ra2snes PRIVATE Qt${QT_VERSION_MAJOR}::WebSockets)
target_link_libraries(ra2snes PRIVATE Qt${QT_VERSION_MAJOR}::Quick)
target_link_libraries(ra2snes PRIVATE Qt${QT_VERSION_MAJOR}::Qml)
target_link_libraries(ra2snes PRIVATE Qt${QT_VERSION_MAJOR}::QuickWidgets)

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ra2snes)
endif()
set_target_properties(ra2snes PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ra2snes
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ra2snes)
endif()
