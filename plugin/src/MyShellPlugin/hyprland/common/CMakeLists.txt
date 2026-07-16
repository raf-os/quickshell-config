qt6_add_library(nightshell_hyprland_common STATIC hyprdefs.h)

target_include_directories(
  nightshell_hyprland_common
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(nightshell_hyprland_common PRIVATE Qt6::Core)
