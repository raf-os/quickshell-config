find_package(Qt6 REQUIRED COMPONENTS Core Qml Network)

qt6_add_library(ns_wl_hyprland STATIC)

target_sources(
  ns_wl_hyprland
  PRIVATE
    hyprland.cpp
    hyprctl.cpp
    hyprevents.cpp
    hypr_monitor.cpp
    hypr_workspace.cpp
    hypr_configs.cpp
)

qt_declare_logging_category(
  ns_wl_hyprland
  IDENTIFIER "ns::wayland::hyprland::NS_LOG_HYPRLAND"
  HEADER "ns_hyprland_debug.h"
  CATEGORY_NAME "ns.wayland.hyprland"
  DEFAULT_SEVERITY Debug
)

target_include_directories(
  ns_wl_hyprland
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(
  ns_wl_hyprland
  PRIVATE
    Qt6::Core
    Qt6::Qml
    Qt6::Network
    nightshell_wayland_workspaces
    nightshell_utils
    ns_core
  PUBLIC nightshell_wayland_common
)
