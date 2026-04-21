find_package(Qt6 REQUIRED COMPONENTS
  Core
  Qml
  Gui
  Quick)

qt_add_executable(MyShellControlPanel
    main.cpp)

file(GLOB_RECURSE
  QML_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/qml/*.qml")

qt_add_qml_module(MyShellControlPanel
  URI MyShellControlPanel
  QML_FILES
    Main.qml
    ${QML_FILES})

add_subdirectory(components)

set_target_properties(MyShellControlPanel
  PROPERTIES
    OUTPUT_NAME "myshell-control-panel"
    INSTALL_RPATH "${INSTALL_QMLDIR}/${MYSHELL_QML_TARGET_PATH}")

target_link_libraries(MyShellControlPanel
    PRIVATE
      myshell
      panel_components
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
