find_package(Qt6 REQUIRED COMPONENTS Core)

qt6_add_library(nightshell_wayland_common STATIC)

target_sources(nightshell_wayland_common PRIVATE compositor_support.cpp)

target_include_directories(
  nightshell_wayland_common
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
