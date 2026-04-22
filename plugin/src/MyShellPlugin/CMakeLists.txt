find_package(Qt6 REQUIRED COMPONENTS
  Core
  Qml
  Gui
  Quick
  Sql
  DBus
)

find_package(LibXml2 REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(HYPRLANG REQUIRED hyprlang)

set(QT_QML_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml")

FetchContent_Declare(rapidfuzz
  GIT_REPOSITORY https://github.com/rapidfuzz/rapidfuzz-cpp.git
  GIT_TAG v3.3.3)
FetchContent_MakeAvailable(rapidfuzz)

add_subdirectory(include)

# This function was taken from Caelestia, which was likely adapted from Quickshell, which then was adapted from KDE ECM
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

  string(TOUPPER ${arg_TARGET} target_upper)
  set("${target_upper}_QML_TARGET_PATH"
    "${module_target_path}"
    CACHE INTERNAL "")

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

add_subdirectory(interfaces)
add_subdirectory(configs)
add_subdirectory(utils)

qml_module(myshell
  URI MyShellPlugin
  SOURCES
    appdb.h appdb.cpp
    cmdhandler.h cmdhandler.cpp
    kbd.h kbd.cpp
    hypr.h hypr.cpp
    networkservice.h networkservice.cpp
    colors.h colors.cpp
  LIBRARIES
    Qt::Gui
    Qt::Quick
    Qt::Sql
    Qt::DBus
    rapidfuzz::rapidfuzz
    LibXml2::LibXml2
    nm_dbus
    myshell_include
    myshell_utils
    ${HYPRLANG_LIBRARIES}
)

target_include_directories(myshell PRIVATE ${HYPRLANG_INCLUDE_DIRS})
target_compile_options(myshell PRIVATE ${HYPRLANG_CFLAGS_OTHER})

