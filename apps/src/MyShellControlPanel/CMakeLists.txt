find_package(Qt6 REQUIRED COMPONENTS
  Core
  Qml
  Gui
  Quick
  Widgets)

qt_add_executable(MyShellControlPanel
    main.cpp)

message("${MYSHELL_QML_TARGET_PATH}")

set_target_properties(MyShellControlPanel
  PROPERTIES
    OUTPUT_NAME "myshell-control-panel"
    INSTALL_RPATH "${INSTALL_QMLDIR}/${MYSHELL_QML_TARGET_PATH}")

target_link_libraries(MyShellControlPanel
    PRIVATE
    myshell
      Qt::Core
      Qt::Qml
      Qt::Gui
      Qt::Quick
      Qt::Widgets)

include(GNUInstallDirs)
install(TARGETS MyShellControlPanel
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
