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

target_compile_definitions(
  nightshell_wayland_toplevels
  PRIVATE NS_WAYLAND_SHARED_LIB
)

target_link_libraries(
  nightshell_wayland_toplevels
  PRIVATE Qt6::Core Qt6::Qml Qt6::WaylandClient Qt6::WaylandClientPrivate
  PUBLIC wlp-ext-foreign-toplevel
)

include(GNUInstallDirs)
install(
  TARGETS nightshell_wayland_toplevels
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
