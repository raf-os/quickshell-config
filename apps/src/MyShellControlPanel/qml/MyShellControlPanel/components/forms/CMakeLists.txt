file(GLOB
  QML_COMPONENTS
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.qml")

cp_add_module(panel_components_forms
  URI MyShellControlPanel.components.forms
  QML_FILES
    ${QML_COMPONENTS}
)
