find_package(
  Qt6
  REQUIRED
  COMPONENTS Core Gui Qml Quick ShaderTools
)

qt6_add_library(nightshell_components)

target_include_directories(
  nightshell_components
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(
  nightshell_components
  PRIVATE chamferrectshader.cpp chamferrectmaterial.cpp chamferrect.cpp
)

target_link_libraries(
  nightshell_components
  PRIVATE Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick Qt6::ShaderTools
)
