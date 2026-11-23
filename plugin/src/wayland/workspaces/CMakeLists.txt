find_package(
  Qt6
  REQUIRED
  COMPONENTS Core Qml Quick WaylandClient WaylandClientPrivate
)

qt6_add_library(nightshell_wayland_workspaces STATIC)

target_sources(
  nightshell_wayland_workspaces
  PRIVATE workspace_manager.cpp workspace_handle.cpp
)

target_include_directories(
  nightshell_wayland_workspaces
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
)

set_target_properties(
  nightshell_wayland_workspaces
  PROPERTIES POSITION_INDEPENDENT_CODE ON
)

gen_wayland_protocol(
  wlp_ext_workspace
  ext-workspace-v1
  "${WAYLAND_PROTOCOLS}/staging/ext-workspace"
)

qt_declare_logging_category(
  nightshell_wayland_workspaces
  IDENTIFIER "ns::wayland::workspaces::NS_LOG_WAYLAND_WORKSPACES"
  HEADER "ns_wayland_workspaces_debug.h"
  CATEGORY_NAME "ns.wayland.workspaces"
  DEFAULT_SEVERITY Debug
)

qt_generate_export_macros(
  nightshell_wayland_workspaces
  HEADER "ns_wayland_workspaces_export.h"
  NAME "NS_WAYLAND_WORKSPACES"
)

target_link_libraries(
  nightshell_wayland_workspaces
  PRIVATE Qt6::Core Qt6::Qml Qt6::WaylandClient Qt6::WaylandClientPrivate
  PUBLIC wlp_ext_workspace nightshell_wayland_common
)
