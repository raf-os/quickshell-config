qt_add_library(panel_singletons SHARED)

file(GLOB_RECURSE
  QML_COMPONENTS
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.qml")

set(qml_singletons
  Hypr.qml
)

set_source_files_properties(${qml_singletons}
  PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

cp_add_module(panel_singletons
  URI MyShellControlPanel.singletons
  QML_FILES
    ${QML_COMPONENTS})
