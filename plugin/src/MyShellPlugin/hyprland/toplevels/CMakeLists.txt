find_package(
  Qt6
  REQUIRED
  COMPONENTS Core Qml Quick WaylandClient WaylandClientPrivate
)
# find_package(WaylandScanner REQUIRED)
# pkg_check_modules(
#   WAYLAND_CLIENT
#   REQUIRED
#   IMPORTED_TARGET
#   wayland-client
#   wayland-protocols>=1.41
# )

qt6_add_library(
  myshell_hyprland_toplevels
  STATIC
  toplevelmanager.cpp
  toplevelhandle.cpp
  hyprtoplevelmanager.cpp
  hyprtoplevelhandle.cpp
)

target_include_directories(
  myshell_hyprland_toplevels
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

qt6_add_qml_module(
  myshell_hyprland_toplevels
  URI "${ORG_URI_PREFIX}.Hyprland.Toplevels"
  VERSION 0.1
  DEPENDENCIES QtQml
)

gen_wayland_protocol(
  wlp-foreign-toplevel
  wlr-foreign-toplevel-management-unstable-v1
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

gen_wayland_protocol(
  wlp-hyprland-toplevel-mapping
  hyprland-toplevel-mapping-v1
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

set_target_properties(
  wlp-foreign-toplevel
  PROPERTIES POSITION_INDEPENDENT_CODE ON
)
set_target_properties(
  wlp-hyprland-toplevel-mapping
  PROPERTIES POSITION_INDEPENDENT_CODE ON
)
set_target_properties(
  myshell_hyprland_toplevels
  PROPERTIES POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(
  myshell_hyprland_toplevels
  PRIVATE
    Qt6::Core
    Qt6::Qml
    Qt6::Quick
    Qt6::WaylandClient
    Qt6::WaylandClientPrivate
  PUBLIC wlp-foreign-toplevel wlp-hyprland-toplevel-mapping
)

install_qml_module(myshell_hyprland_toplevels)
