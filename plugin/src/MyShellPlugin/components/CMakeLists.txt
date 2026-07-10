find_package(
  Qt6
  REQUIRED
  COMPONENTS Core Qml Quick Svg Xml ShaderTools
)

qt6_add_library(myshell_components SHARED)

target_compile_definitions(
  myshell_components
  PUBLIC DATADIR_DEFAULT_PATH="${MYSHELL_INSTALL_DATADIR}"
)

target_include_directories(
  myshell_components
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(myshell_components PRIVATE svgicon.cpp)

qt6_add_qml_module(
  myshell_components
  URI "${ORG_URI_PREFIX}.Components"
  VERSION 0.1
  QML_FILES RoundedClipRect.qml
)

qt6_add_shaders(myshell_components "components-rectclipmask"
  NOHLSL NOMSL BATCHABLE PRECOMPILE OPTIMIZED QUIET
  PREFIX "/Nightshell/Components"
  FILES shaders/rectclipmask.frag
  OUTPUTS shaders/rectclipmask.frag.qsb
)

target_link_libraries(
  myshell_components
  PRIVATE
    Qt6::Core
    Qt6::Qml
    Qt6::Quick
    Qt6::Qml
    Qt6::Svg
    Qt6::Xml
    Qt6::ShaderTools
)

install_qml_module(myshell_components)

# qml_module(myshell_components
#   URI "${ORG_URI_PREFIX}.Components"
#   SOURCES
#     svgicon.h svgicon.cpp
#   LIBRARIES
#     Qt::Quick
#     Qt::Svg
#     Qt::Xml
# )
