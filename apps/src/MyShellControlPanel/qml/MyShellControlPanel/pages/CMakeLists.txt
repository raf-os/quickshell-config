set(MODULE_PAGES
  colors
  keyboard)

file(GLOB
  QML_COMPONENTS
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.qml")

cp_add_module(panel_pages
  URI MyShellControlPanel.pages
  QML_FILES
    ${QML_COMPONENTS})

foreach(X ${MODULE_PAGES})
  file(GLOB
    QML_COMPONENTS
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/${X}/*.qml")
  cp_add_module("panel_pages_${X}"
    URI "MyShellControlPanel.pages.${X}"
    QML_FILES
      ${QML_COMPONENTS}
  )
  # target_link_libraries(panel_pages
  #   PRIVATE
  #     "panel_pages_${X}")
endforeach()
