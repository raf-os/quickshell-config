find_package(Qt6 REQUIRED COMPONENTS Core)

qt6_add_library(nightshell_wayland_common STATIC)

target_sources(nightshell_wayland_common PRIVATE compositor_support.cpp)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/wayland_common_config.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/wayland_common_config.h"
)

target_include_directories(
  nightshell_wayland_common
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
)
