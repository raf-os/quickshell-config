# Credits to KDE's layer-shell-qt, most of the code was adapted from here,
# with minor modifications:
# https://github.com/KDE/layer-shell-qt/tree/master

gen_wayland_protocol(
  wlr_layer_shell
  wlr-layer-shell-unstable-v1
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

qt6_add_library(ns_wlr_layer_shell SHARED)

set_target_properties(
  ns_wlr_layer_shell
  PROPERTIES POSITION_INDEPENDENT_CODE ON
)

target_sources(
  ns_wlr_layer_shell
  PRIVATE
    layer_shell_integration.cpp
    layer_enums.cpp
    layer_surface.cpp
    layer_window.cpp
)

qt_declare_logging_category(
  ns_wlr_layer_shell
  IDENTIFIER "ns::layershell::NS_LOG_LAYERSHELL"
  HEADER "ns_layershell_debug.h"
  CATEGORY_NAME "ns.layershell"
  DEFAULT_SEVERITY Debug
)

qt_generate_export_macros(
  ns_wlr_layer_shell
  HEADER "ns_layershell_export.h"
  NAME "NS_LAYERSHELL"
)

target_link_libraries(
  ns_wlr_layer_shell
  PRIVATE
    Qt6::Core
    Qt6::Qml
    Qt6::WaylandClient
    Qt6::WaylandClientPrivate
    wlr_layer_shell
    ns_xdg_activation
)
