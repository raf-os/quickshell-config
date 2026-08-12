find_package(
  Qt6
  REQUIRED
  COMPONENTS Core Qml Quick WaylandClient WaylandClientPrivate
)

qt6_add_library(nightshell_wayland_wlr STATIC)

target_sources(
  nightshell_wayland_wlr
  PRIVATE toplevelhandle.cpp toplevelmanager.cpp
)

target_include_directories(
  nightshell_wayland_wlr
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

qt6_add_qml_module(
  nightshell_wayland_wlr
  URI "${ORG_URI_PREFIX}.Wayland.Wlr"
  VERSION 0.1
  DEPENDENCIES QtQml
)

# gen_wayland_protocol(
#   wlp-ext-foreign-toplevel
#   ext-foreign-toplevel-list-v1
#   "${WAYLAND_PROTOCOLS}/staging/ext-foreign-toplevel-list"
# )

gen_wayland_protocol(
  wlp-foreign-toplevel
  wlr-foreign-toplevel-management-unstable-v1
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

set_target_properties(
  nightshell_wayland_wlr
  PROPERTIES POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(
  nightshell_wayland_wlr
  PRIVATE
    Qt6::Core
    Qt6::Qml
    Qt6::Quick
    Qt6::WaylandClient
    Qt6::WaylandClientPrivate
  PUBLIC wlp-foreign-toplevel nightshell_wayland_toplevels
)

install_qml_module(nightshell_wayland_wlr)
