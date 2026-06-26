find_package(Qt6 REQUIRED COMPONENTS Core Qml Gui Qml Quick)

set(QT_QML_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml")

qt_add_library(nightshell_wallpaper SHARED wallpapermanager.cpp
               wallpapermeta.cpp wallpapercommon.cpp)

target_include_directories(
  nightshell_wallpaper
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

qt_add_qml_module(nightshell_wallpaper URI "${ORG_URI_PREFIX}.Wallpaper"
                  VERSION 0.1)

target_link_libraries(nightshell_wallpaper PRIVATE Qt::Core Qt::Gui Qt::Qml
                                                   myshell_utils)

install_qml_module(nightshell_wallpaper)
