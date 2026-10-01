find_package(Qt6 REQUIRED COMPONENTS Core Qml Sql)

set(QT_QML_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml")

qt6_add_library(nightshell_desktop_entries SHARED)

target_sources(
  nightshell_desktop_entries
  PRIVATE
    entryaction.cpp
    entrymanager.cpp
    entrycacher.cpp
    entryscanner.cpp
    entryutils.cpp
    entrymonitor.cpp
    localeutil.cpp
    desktopentry.cpp
    desktopentriesmodel.cpp
)

target_include_directories(
  nightshell_desktop_entries
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(
  nightshell_desktop_entries
  PRIVATE NS_DESKTOPENTRIES_SHARED_LIB
)

qt6_add_qml_module(
  nightshell_desktop_entries
  URI "${ORG_URI_PREFIX}.DesktopEntries"
  VERSION 0.1
  DEPENDENCIES "${ORG_URI_PREFIX}.Utils"
)

target_link_libraries(
  nightshell_desktop_entries
  PRIVATE
    Qt6::Core
    Qt6::Qml
    Qt6::Sql
    rapidfuzz::rapidfuzz
    nightshell_utils
    nightshell_includes
)

install_qml_module(nightshell_desktop_entries)
