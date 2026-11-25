find_package(Qt6 REQUIRED COMPONENTS Core Qml Gui)

set(QT_QML_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml")

add_subdirectory(includes)
add_subdirectory(utils)
add_subdirectory(configs)
add_subdirectory(interfaces)

qt6_add_library(ns_core STATIC)

target_include_directories(ns_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(
  ns_core
  PRIVATE shell_global.cpp object_repeater.cpp screen_info.cpp
)

qt6_add_qml_module(
  ns_core
  URI "${ORG_URI_PREFIX}"
  VERSION 0.1
  PLUGIN_TARGET ns_core_plugin
)

target_link_libraries(
  ns_core
  PRIVATE Qt6::Core Qt6::Gui Qt6::Qml
  PUBLIC nightshell_wayland_common
)
