find_package(Qt6 REQUIRED COMPONENTS
  Core
  Qml
  Quick
  Sql
)

set(QT_QML_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml")
qt_standard_project_setup(REQUIRES 6.9)

function(qml_module arg_TARGET)
  cmake_parse_arguments(PARSE_ARGV 1 arg "" "URI" "SOURCES;LIBRARIES")

  qt_add_qml_module(${arg_TARGET}
    URI ${arg_URI}
    VERSION ${VERSION}
    SOURCES ${arg_SOURCES}
  )

  qt_query_qml_module(${arg_TARGET}
    URI module_uri
    VERSION module_version
    PLUGIN_TARGET module_plugin_target
    TARGET_PATH module_target_path
    QMLDIR module_qmldir
    TYPEINFO module_typeinfo
  )

  target_link_libraries(${arg_TARGET}
    PRIVATE
      Qt::Core
      Qt::Qml
      ${arg_LIBRARIES}
    )
  message(STATUS "Created QML module ${module_uri}, version ${module_version}")
endfunction()

qml_module(${PROJECT_NAME}
  URI MyShellPlugin
  SOURCES
    appdb.h appdb.cpp
  LIBRARIES
    Qt::Gui
    Qt::Quick
    Qt::Sql
)
