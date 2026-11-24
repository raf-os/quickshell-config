configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/.qmlls.ini.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/.qmlls.ini"
)

qt6_add_library(ns_shell_root STATIC)

qt6_add_qml_module(ns_shell_root URI "ns" VERSION 0.1 QML_FILES Main.qml)
