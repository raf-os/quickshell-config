gen_wayland_protocol(
  wlp_xdg_shell
  xdg-shell
  "${WAYLAND_PROTOCOLS}/stable/xdg-shell"
)

gen_wayland_protocol(
  wlp_xdg_activation
  xdg-activation-v1
  "${WAYLAND_PROTOCOLS}/staging/xdg-activation"
)

qt6_add_library(ns_xdg_activation STATIC)

set_target_properties(ns_xdg_activation PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(ns_xdg_activation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(
  ns_xdg_activation
  PRIVATE xdg_activation.cpp xdg_activation_token.cpp
)

target_link_libraries(
  ns_xdg_activation
  PRIVATE Qt6::Core Qt6::WaylandClient Qt6::WaylandClientPrivate
  PUBLIC wlp_xdg_shell wlp_xdg_activation
)
