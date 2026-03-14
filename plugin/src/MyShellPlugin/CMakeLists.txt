find_package(Qt6 REQUIRED COMPONENTS
  Core
  Qml
  Gui
  Quick
  Sql
)

set(QT_QML_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml")
qt_standard_project_setup(REQUIRES 6.9)

FetchContent_Declare(rapidfuzz
  GIT_REPOSITORY https://github.com/rapidfuzz/rapidfuzz-cpp.git
  GIT_TAG v3.3.3)
FetchContent_MakeAvailable(rapidfuzz)

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

  message(STATUS "Created QML module ${module_uri}, version ${module_version}")

  set(module_dir "${INSTALL_QMLDIR}/${module_target_path}")
  install(TARGETS ${arg_TARGET}
    LIBRARY DESTINATION "${module_dir}"
    RUNTIME DESTINATION "${module_dir}"
  )
  install(TARGETS ${module_plugin_target}
    LIBRARY DESTINATION "${module_dir}"
    RUNTIME DESTINATION "${module_dir}"
  )
  install(FILES "${module_qmldir}" DESTINATION "${module_dir}")
  install(FILES "${module_typeinfo}" DESTINATION "${module_dir}")

  target_link_libraries(${arg_TARGET}
    PRIVATE
      Qt::Core
      Qt::Qml
      ${arg_LIBRARIES}
    )
endfunction()

qml_module(myshell
  URI MyShellPlugin
  SOURCES
    appdb.h appdb.cpp
    cmdhandler.h cmdhandler.cpp
  LIBRARIES
    Qt::Gui
    Qt::Quick
    Qt::Sql
    rapidfuzz::rapidfuzz
)
