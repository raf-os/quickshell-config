find_package(Qt6 REQUIRED COMPONENTS Core Qml)

qt6_add_library(ns_wl_hyprland STATIC)

target_sources(
  ns_wl_hyprland
  PRIVATE hyprland.cpp hypr_monitor.cpp hypr_workspace.cpp
)

target_include_directories(ns_wl_hyprland PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(
  ns_wl_hyprland
  PRIVATE Qt6::Core Qt6::Qml nightshell_wayland_workspaces
  PUBLIC nightshell_wayland_common
)
