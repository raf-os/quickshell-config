find_package(Qt6 REQUIRED COMPONENTS
  Core
  Qml
  Gui
  Quick)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/.qmlls.ini.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/.qmlls.ini"
)

qt_add_executable(MyShellControlPanel
    main.cpp)

file(GLOB_RECURSE
  QML_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/qml/*.qml")



function(cp_add_module arg_TARGET)
  cmake_parse_arguments(arg "" "URI" "QML_FILES;SOURCES" ${ARGN})

  string(REPLACE "." "/" URI_PATH ${arg_URI})

  qt_add_qml_module(${arg_TARGET}
    URI ${arg_URI}
    OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml/${URI_PATH}"
    QML_FILES ${arg_QML_FILES}
    SOURCES ${arg_SOURCES})
endfunction()

cp_add_module(MyShellControlPanel
  URI MyShellControlPanel
  QML_FILES
    Main.qml
    ${QML_FILES})

add_subdirectory(components)
add_subdirectory(singletons)

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
