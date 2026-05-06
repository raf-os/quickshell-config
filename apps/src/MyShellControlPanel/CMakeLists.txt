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

function(cp_add_module arg_TARGET)
  cmake_parse_arguments(arg "" "URI" "QML_FILES;SOURCES" ${ARGN})

  string(REPLACE "." "/" URI_PATH ${arg_URI})

  qt_add_qml_module(${arg_TARGET}
    URI ${arg_URI}
    OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml/${URI_PATH}"
    QML_FILES ${arg_QML_FILES}
    SOURCES ${arg_SOURCES})
endfunction()

add_subdirectory(qml/MyShellControlPanel)
