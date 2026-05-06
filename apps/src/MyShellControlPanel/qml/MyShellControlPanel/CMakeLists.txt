file(GLOB
  QML_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.qml")


cp_add_module(MyShellControlPanel
  URI MyShellControlPanel
  QML_FILES
    Main.qml
    ${QML_FILES})

add_subdirectory(components)
add_subdirectory(singletons)
add_subdirectory(pages)

set_target_properties(MyShellControlPanel
  PROPERTIES
    OUTPUT_NAME "myshell-control-panel"
    INSTALL_RPATH "${INSTALL_QMLDIR}/${MYSHELL_QML_TARGET_PATH}")

target_compile_definitions(MyShellControlPanel PRIVATE
  SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

target_compile_definitions(MyShellControlPanel PRIVATE DEBUG)

target_link_libraries(MyShellControlPanel
    PRIVATE
      myshell
      panel_components
      panel_singletons
      panel_pages
      Qt::Core
      Qt::Qml
      Qt::Gui
      Qt::Quick)

include(GNUInstallDirs)
install(TARGETS MyShellControlPanel
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
