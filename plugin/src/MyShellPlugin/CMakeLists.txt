find_package(
  Qt6
  REQUIRED
  COMPONENTS Core Qml Gui Quick Sql Network DBus
)

find_package(LibXml2 REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBCANBERRA REQUIRED IMPORTED_TARGET libcanberra)

set(QT_QML_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml")

FetchContent_Declare(
  rapidfuzz
  GIT_REPOSITORY https://github.com/rapidfuzz/rapidfuzz-cpp.git
  GIT_TAG v3.3.3
)
FetchContent_MakeAvailable(rapidfuzz)

add_subdirectory(include)

qt6_add_library(myshell SHARED)

target_sources(
  myshell
  PRIVATE
    appdb.cpp
    cmdhandler.cpp
    kbd.cpp
    hypr.cpp
    networkservice.cpp
    colors.cpp
    systemsounds.cpp
)

target_include_directories(myshell PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

qt6_add_qml_module(myshell URI "MyShellPlugin" VERSION 0.1)

add_subdirectory(interfaces)
add_subdirectory(configs)
add_subdirectory(utils)
add_subdirectory(components)
add_subdirectory(notifications)
add_subdirectory(wallpaper)
add_subdirectory(hyprland)

target_link_libraries(
  myshell
  PRIVATE
    Qt6::Core
    Qt6::Qml
    Qt6::Quick
    Qt6::Sql
    Qt6::DBus
    Qt6::Network
    rapidfuzz::rapidfuzz
    LibXml2::LibXml2
    PkgConfig::LIBCANBERRA
    myshell_utils
    myshell_interfaces
    myshell_include
    myshell_configs
    myshell_hyprland
)

install_qml_module(myshell)
