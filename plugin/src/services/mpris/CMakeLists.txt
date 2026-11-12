find_package(Qt6 REQUIRED COMPONENTS Core Qml DBus)

qt6_add_library(nightshell_mpris SHARED)

set_source_files_properties(
  org.mpris.MediaPlayer2.xml
  PROPERTIES CLASSNAME DBusMprisMediaPlayer
)

qt6_add_dbus_interface(
  MPRIS_DBUS_INTERFACES
  org.mpris.MediaPlayer2.xml
  dbus_mpris
)

target_include_directories(
  nightshell_mpris
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(
  nightshell_mpris
  PRIVATE mpris_service_watcher.cpp mpris_player.cpp ${MPRIS_DBUS_INTERFACES}
)

qt_declare_logging_category(
  nightshell_mpris
  IDENTIFIER "ns::mpris::NS_LOG_MPRIS"
  HEADER "ns_mpris_debug.h"
  CATEGORY_NAME "ns.mpris"
  DEFAULT_SEVERITY Debug
)

qt_generate_export_macros(nightshell_mpris
  HEADER "ns_mpris_export.h"
  NAME "NS_MPRIS"
)

set_target_properties(nightshell_mpris PROPERTIES POSITION_INDEPENDENT_CODE ON)

qt6_add_qml_module(nightshell_mpris URI "${ORG_URI_PREFIX}.Mpris" VERSION 0.1)

target_link_libraries(
  nightshell_mpris
  PRIVATE Qt6::Core Qt6::Qml Qt6::DBus nightshell_includes nightshell_dbus
)

install_qml_module(nightshell_mpris)
