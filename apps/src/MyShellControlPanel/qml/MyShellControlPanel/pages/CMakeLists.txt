set(MODULE_PAGES
  colors
  keyboard)

qt_add_library(panel_pages SHARED)

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
  qt_add_library("panel_pages_${X}" SHARED)
  file(GLOB
    QML_COMPONENTS
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/${X}/*.qml")
  cp_add_module("panel_pages${X}"
    URI "MyShellControlPanel.pages.${X}"
    QML_FILES
      ${QML_COMPONENTS})
endforeach()
