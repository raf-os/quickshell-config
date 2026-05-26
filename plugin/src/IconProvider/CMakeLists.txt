find_package(Qt6 REQUIRED COMPONENTS
  Core
  Qml
  Gui
  Quick
)

set(QT_QML_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml")

function(qml_plugin arg_TARGET)
  cmake_parse_arguments(PARSE_ARGV 1 arg "" "URI" "SOURCES;LIBRARIES")

  qt_add_qml_module(${arg_TARGET}
    URI ${arg_URI}
    VERSION 1.0
    PLUGIN_TARGET ${arg_TARGET}
    NO_PLUGIN_OPTIONAL
    NO_GENERATE_PLUGIN_SOURCE
  )

  target_sources(${arg_TARGET} PRIVATE
    ${arg_SOURCES})

  qt_query_qml_module(${arg_TARGET}
    URI module_uri
    VERSION module_version
    PLUGIN_TARGET module_plugin_target
    TARGET_PATH module_target_path
    QMLDIR module_qmldir
    TYPEINFO module_typeinfo
  )

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
      Qt::Quick
      ${arg_LIBRARIES}
  )
endfunction()

qml_plugin(icon_provider_plugin
  URI "${ORG_URI_PREFIX}.IconProvider"
  SOURCES
    iconprovider.h iconprovider.cpp
)
