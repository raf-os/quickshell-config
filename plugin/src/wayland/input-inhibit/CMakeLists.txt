find_package(
  Qt6
  REQUIRED
  COMPONENTS Core Qml Quick WaylandClient WaylandClientPrivate
)

qt6_add_library(nightshell_input_inhibit SHARED)

target_sources(
  nightshell_input_inhibit
  PRIVATE input_inhibit_manager.cpp shortcut_inhibitor.cpp qmlinputinhibitor.cpp
)

target_include_directories(
  nightshell_input_inhibit
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

gen_wayland_protocol(
  wlp_keyboard_shortcuts_inhibit
  keyboard-shortcuts-inhibit-unstable-v1
  "${WAYLAND_PROTOCOLS}/unstable/keyboard-shortcuts-inhibit"
)

set_target_properties(
  nightshell_input_inhibit
  PROPERTIES POSITION_INDEPENDENT_CODE ON
)

set_target_properties(
  wlp_keyboard_shortcuts_inhibit
  PROPERTIES POSITION_INDEPENDENT_CODE ON
)

target_compile_definitions(
  nightshell_input_inhibit
  PRIVATE NS_WL_INPUT_INHIBIT_SHARED
)

qt6_add_qml_module(
  nightshell_input_inhibit
  URI "${ORG_URI_PREFIX}.Wayland.InputInhibit"
  VERSION 0.1
  DEPENDENCIES QtQml QtQuick
)

target_link_libraries(
  nightshell_input_inhibit
  PRIVATE
    Qt6::Core
    Qt6::Qml
    Qt6::Quick
    Qt6::WaylandClient
    Qt6::WaylandClientPrivate
  PUBLIC wlp_keyboard_shortcuts_inhibit
)

install_qml_module(nightshell_input_inhibit)
