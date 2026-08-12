find_package(Qt6 REQUIRED COMPONENTS Core Qml Quick)

qt6_add_library(nightshell_wayland_screencopy STATIC)

target_sources(
  nightshell_wayland_screencopy
  PRIVATE screencopy_context.cpp screencopy_manager.cpp
)

target_include_directories(
  nightshell_wayland_screencopy
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(wlr_screencopy)
add_subdirectory(image_copy_capture)

target_link_libraries(
  nightshell_wayland_screencopy
  PRIVATE Qt6::Core Qt6::Quick
  PUBLIC
    nightshell_wayland_buffer
    nightshell_wayland_toplevels
    nightshell_wayland_screencopy_icc
)
