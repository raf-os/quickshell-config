find_package(Qt6 REQUIRED COMPONENTS Core Qml Gui Quick)

find_package(LibXml2 REQUIRED)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/.qmlls.ini.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/.qmlls.ini"
)

qt_add_executable(myshell_control_panel main.cpp)

function(cp_add_module arg_TARGET)
  cmake_parse_arguments(PARSE_ARGV 1 arg "" "URI" "QML_FILES;SOURCES;LIBRARIES")

  string(REPLACE "." "/" URI_PATH ${arg_URI})

  qt_add_library(${arg_TARGET} STATIC)

  qt_add_qml_module(
    ${arg_TARGET}
    URI
    ${arg_URI}
    OUTPUT_DIRECTORY
    "${CMAKE_BINARY_DIR}/qml/${URI_PATH}"
    QML_FILES
    ${arg_QML_FILES}
    SOURCES
    ${arg_SOURCES}
  )

  target_link_libraries(${arg_TARGET} PRIVATE ${arg_LIBRARIES})

  target_link_libraries(
    myshell_control_panel
    PRIVATE ${arg_TARGET} ${arg_TARGET}plugin
  )
endfunction()

include(GNUInstallDirs)

add_subdirectory(plugin)
add_subdirectory(qml/MyShellControlPanel)

set_target_properties(
  myshell_control_panel
  PROPERTIES
    OUTPUT_NAME "myshell-control-panel"
    INSTALL_RPATH
      "${INSTALL_QMLDIR}/${MYSHELL_QML_TARGET_PATH};${INSTALL_QMLDIR}/${MYSHELL_CONFIG_QML_TARGET_PATH}"
)

target_compile_definitions(
  myshell_control_panel
  PRIVATE SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/qml"
)

if(DEVMODE)
  target_compile_definitions(myshell_control_panel PRIVATE DEBUG)
endif()

target_link_libraries(
  myshell_control_panel
  PRIVATE
    panel_root
    panel_components
    panel_pages
    panel_plugin
    Qt::Core
    Qt::Qml
    Qt::Gui
    Qt::Quick
)

install(
  TARGETS myshell_control_panel
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
