find_package(Qt6 REQUIRED COMPONENTS Core Qml)

set(QT_QML_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml")

qt6_add_library(nightshell_desktop_entries SHARED)

target_sources(
  nightshell_desktop_entries
  PRIVATE
    entrymanager.cpp
    entrycacher.cpp
    entryscanner.cpp
    entryutils.cpp
    entrymonitor.cpp
    localeutil.cpp
    desktopentry.cpp
)

target_include_directories(
  nightshell_desktop_entries
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

qt6_add_qml_module(
  nightshell_desktop_entries
  URI "${ORG_URI_PREFIX}.DesktopEntries"
  VERSION 0.1
)

target_link_libraries(
  nightshell_desktop_entries
  PRIVATE Qt::Core Qt::Qml myshell_utils
)

install_qml_module(nightshell_desktop_entries)
