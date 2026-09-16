find_package(Qt6 REQUIRED COMPONENTS Core Qml)

qt6_add_library(nightshell_includes INTERFACE)

target_link_libraries(nightshell_includes INTERFACE Qt6::Core Qt6::Qml)

target_include_directories(
  nightshell_includes
  INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)
