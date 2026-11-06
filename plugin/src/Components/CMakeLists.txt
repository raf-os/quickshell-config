find_package(
  Qt6
  REQUIRED
  COMPONENTS Core Gui Qml Quick ShaderTools
)

qt6_add_library(nightshell_components SHARED)

target_include_directories(
  nightshell_components
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(
  nightshell_components
  PRIVATE
    chamferrectshader.cpp
    chamferrectmaterial.cpp
    chamferrect.cpp
    stooltip.cpp
    tooltip_provider.cpp
)

set_target_properties(
  nightshell_components
  PROPERTIES POSITION_INDEPENDENT_CODE ON
)
target_compile_definitions(
  nightshell_components
  PRIVATE QML_MODULE_URI="${ORG_URI_PREFIX}.Components"
)

qt6_add_qml_module(
  nightshell_components
  URI "${ORG_URI_PREFIX}.Components"
  VERSION 0.1
  QML_FILES RoundedClipRect.qml TooltipTemplate.qml
  DEPENDENCIES "${ORG_URI_PREFIX}.Configs"
)

qt6_add_shaders(nightshell_components "shaders"
  NOHLSL NOMSL BATCHABLE PRECOMPILE OPTIMIZED QUIET
  PREFIX "/Nightshell/Components"
  FILES
    shaders/rectclipmask.frag
    shaders/chamferrect.frag
    shaders/chamferrect.vert
  OUTPUTS
    shaders/rectclipmask.frag.qsb
    shaders/chamferrect.frag.qsb
    shaders/chamferrect.vert.qsb
)

target_link_libraries(
  nightshell_components
  PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::ShaderTools
    nightshell_includes
)

install_qml_module(nightshell_components)
