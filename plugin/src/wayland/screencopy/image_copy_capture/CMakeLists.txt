find_package(
  Qt6
  REQUIRED
  COMPONENTS Core Qml Quick WaylandClient WaylandClientPrivate
)

qt6_add_library(nightshell_wayland_screencopy_icc STATIC)

target_sources(
  nightshell_wayland_screencopy_icc
  PRIVATE iccmanager.cpp icc_screencopy_context.cpp
)

target_include_directories(
  nightshell_wayland_screencopy_icc
  PRIVATE "${CMAKE_CURRENT_BINARY_DIR}"
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)

gen_wayland_protocol(
  wlp-ext-foreign-toplevel
  ext-foreign-toplevel-list-v1
  "${WAYLAND_PROTOCOLS}/staging/ext-foreign-toplevel-list"
)

gen_wayland_protocol(
  wlp-image-copy-capture
  ext-image-copy-capture-v1
  "${WAYLAND_PROTOCOLS}/staging/ext-image-copy-capture"
)

gen_wayland_protocol(
  wlp-image-capture-source
  ext-image-capture-source-v1
  "${WAYLAND_PROTOCOLS}/staging/ext-image-capture-source"
)

set_target_properties(
  wlp-ext-foreign-toplevel
  PROPERTIES POSITION_INDEPENDENT_CODE ON
)
set_target_properties(
  wlp-image-copy-capture
  PROPERTIES POSITION_INDEPENDENT_CODE ON
)
set_target_properties(
  wlp-image-capture-source
  PROPERTIES POSITION_INDEPENDENT_CODE ON
)
set_target_properties(
  nightshell_wayland_screencopy_icc
  PROPERTIES POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(
  nightshell_wayland_screencopy_icc
  PRIVATE Qt6::Core Qt6::WaylandClient Qt6::WaylandClientPrivate Qt6::Quick
  PUBLIC
    wlp-image-copy-capture
    wlp-image-capture-source
    wlp-ext-foreign-toplevel
    nightshell_wayland_toplevels
    nightshell_wayland_screencopy
)
