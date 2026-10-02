# NOTE: This specific library requires Qt to be at the very least version 6.10.0
# Considering this makes use of private Qt libraries, breaking changes are to be
# expected.

find_package(
  Qt6
  REQUIRED
  COMPONENTS Core Qml Quick WaylandClient WaylandClientPrivate
)

qt6_add_library(nightshell_wayland_sessionlock STATIC)

target_sources(
  nightshell_wayland_sessionlock
  PRIVATE
    lock_manager.cpp
    session_lock.cpp
    lock_surface.cpp
    window_attached_lock.cpp
    shell_integration.cpp
)

target_include_directories(
  nightshell_wayland_sessionlock
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

gen_wayland_protocol(
  wlp-session-lock
  ext-session-lock-v1
  "${WAYLAND_PROTOCOLS}/staging/ext-session-lock"
)

set_target_properties(
  nightshell_wayland_sessionlock
  PROPERTIES POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(
  nightshell_wayland_sessionlock
  PRIVATE Qt6::Core Qt6::Qml Qt6::WaylandClient Qt6::WaylandClientPrivate
  PUBLIC wlp-session-lock nightshell_includes
)
