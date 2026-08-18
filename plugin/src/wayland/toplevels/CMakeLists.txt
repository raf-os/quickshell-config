find_package(
  Qt6
  REQUIRED
  COMPONENTS Core Qml Quick WaylandClient WaylandClientPrivate
)

qt6_add_library(nightshell_wayland_toplevels SHARED)

target_sources(
  nightshell_wayland_toplevels
  PRIVATE wl_toplevel_handle.cpp wl_toplevel_manager.cpp
)

target_include_directories(
  nightshell_wayland_toplevels
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

gen_wayland_protocol(
  wlp-ext-foreign-toplevel
  ext-foreign-toplevel-list-v1
  "${WAYLAND_PROTOCOLS}/staging/ext-foreign-toplevel-list"
)

set_target_properties(
  nightshell_wayland_toplevels
  PROPERTIES POSITION_INDEPENDENT_CODE ON
)

# This is being done for the hack described below
target_compile_definitions(
  nightshell_wayland_toplevels
  PRIVATE NS_WAYLAND_SHARED_LIB
)

target_link_libraries(
  nightshell_wayland_toplevels
  PRIVATE Qt6::Core Qt6::Qml Qt6::WaylandClient Qt6::WaylandClientPrivate
  PUBLIC wlp-ext-foreign-toplevel
)

# HACK: this is being installed as a regular library, the static / shared barriers between libraries was causing qobject_cast to fail during runtime
include(GNUInstallDirs)
install(
  TARGETS nightshell_wayland_toplevels
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
