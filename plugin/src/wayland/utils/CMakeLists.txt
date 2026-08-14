find_package(Qt6 REQUIRED COMPONENTS Core)

qt6_add_library(nightshell_wayland_util STATIC)

target_sources(nightshell_wayland_util PRIVATE wlprotocolsupport.cpp)

target_include_directories(
  nightshell_wayland_util
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(nightshell_wayland_util PRIVATE Qt6::Core)
