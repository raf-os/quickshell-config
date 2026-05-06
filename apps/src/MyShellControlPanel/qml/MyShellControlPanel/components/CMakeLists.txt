qt_add_library(panel_components SHARED)

file(GLOB_RECURSE
  QML_COMPONENTS
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.qml")

cp_add_module(panel_components
  URI MyShellControlPanel.components
  QML_FILES
    ${QML_COMPONENTS})

# target_link_libraries(MyShellControlPanel
#     PRIVATE
#       panel_components
#       panel_componentsplugin)
