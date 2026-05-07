file(GLOB
  QML_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.qml")


cp_add_module(panel_root
  URI MyShellControlPanel
  QML_FILES
    ${QML_FILES})

add_subdirectory(components)
add_subdirectory(singletons)
add_subdirectory(pages)
