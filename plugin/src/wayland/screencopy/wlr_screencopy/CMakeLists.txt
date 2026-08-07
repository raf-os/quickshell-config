find_package(
  Qt6
  REQUIRED
  COMPONENTS Core Qml Quick WaylandClient WaylandClientPrivate
)

qt6_add_library(nightshell_wayland_screencopy_wlr STATIC)

target_sources(
  nightshell_wayland_screencopy_wlr
  PRIVATE wlr_screencopy.cpp wlr_screencopy_context.cpp
)

gen_wayland_protocol(
  wlp-wlr-screencopy
  wlr-screencopy-unstable-v1
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_include_directories(
  nightshell_wayland_screencopy_wlr
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
  nightshell_wayland_screencopy_wlr
  PRIVATE
    Qt6::WaylandClient
    Qt6::WaylandClientPrivate
    Qt6::Quick
    nightshell_wayland_buffer
  PUBLIC wlp-wlr-screencopy nightshell_wayland_screencopy
)
