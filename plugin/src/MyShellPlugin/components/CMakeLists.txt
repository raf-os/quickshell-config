find_package(Qt6 REQUIRED COMPONENTS
  Core
  Qml
  Quick
  Svg
  Xml
)

qt_add_library(myshell_components SHARED)

target_compile_definitions(myshell_components PUBLIC
  DATADIR_DEFAULT_PATH="${MYSHELL_INSTALL_DATADIR}"
)

qml_module(myshell_components
  URI "${ORG_URI_PREFIX}.Components"
  SOURCES
    svgicon.h svgicon.cpp
  LIBRARIES
    Qt::Quick
    Qt::Svg
    Qt::Xml
)
