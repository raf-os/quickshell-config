find_package(
  Qt6
  REQUIRED
  COMPONENTS Core Gui Qml Quick Network
)

qt6_add_library(lib_hyprland STATIC)

set_target_properties(lib_hyprland PROPERTIES POSITION_INDEPENDENT_CODE ON)

qt_declare_logging_category(
  lib_hyprland
  IDENTIFIER "ns::hyprland::NS_LOG_HYPRLAND"
  HEADER "ns_hyprland_debug.h"
  CATEGORY_NAME "ns.hyprland"
  DEFAULT_SEVERITY Debug
)

qt_generate_export_macros(
  lib_hyprland
  HEADER "ns_hyprland_export.h"
  NAME "NS_HYPRLAND"
)

target_sources(lib_hyprland PRIVATE hyprland.cpp)
