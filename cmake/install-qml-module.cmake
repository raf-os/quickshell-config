find_package(Qt6 REQUIRED COMPONENTS Core Qml)

set(QML_FULL_INSTALLDIR "${INSTALL_QMLDIR}")

# Taken from quickshell
# https://git.outfoxxed.me/quickshell/quickshell/src/branch/master/cmake/install-qml-module.cmake#L20
function(install_qml_module arg_TARGET)
  if(NOT DEFINED QML_FULL_INSTALLDIR)
    return()
  endif()

  qt_query_qml_module(
    ${arg_TARGET}
    URI
    module_uri
    VERSION
    module_version
    PLUGIN_TARGET
    module_plugin_target
    TARGET_PATH
    module_target_path
    QMLDIR
    module_qmldir
    TYPEINFO
    module_typeinfo
    QML_FILES
    module_qml_files
    RESOURCES
    module_resources
  )

  set(module_dir "${QML_FULL_INSTALLDIR}/${module_target_path}")

  if(NOT TARGET "${module_plugin_target}")
    message(
      FATAL_ERROR
      "install_qml_modules called for a target without a plugin."
    )
  endif()

  message(
    STATUS
    "Setup installation for QML module ${module_uri}, version ${module_version}"
  )

  get_target_property(target_type "${arg_TARGET}" TYPE)
  if(NOT "${target_type}" STREQUAL "STATIC_LIBRARY")
    install(
      TARGETS "${arg_TARGET}"
      LIBRARY DESTINATION "${module_dir}"
      RUNTIME DESTINATION "${module_dir}"
    )

    install(
      TARGETS "${module_plugin_target}"
      LIBRARY DESTINATION "${module_dir}"
      RUNTIME DESTINATION "${module_dir}"
    )
  endif()

  install(FILES "${module_qmldir}" DESTINATION "${module_dir}")
  install(FILES "${module_typeinfo}" DESTINATION "${module_dir}")

  # Install QML files, if any
  list(LENGTH module_qml_files num_files)
  if(NOT "${module_qml_files}" MATCHES "NOTFOUND" AND ${num_files} GREATER 0)
    qt_query_qml_module(
      ${arg_TARGET}
      QML_FILES_DEPLOY_PATHS
      qml_files_deploy_paths
    )

    math(EXPR last_index "${num_files} - 1")
    foreach(i RANGE 0 ${last_index})
      list(GET module_qml_files ${i} src_file)
      list(GET qml_files_deploy_paths ${i} deploy_path)
      get_filename_component(dst_name "${deploy_path}" NAME)
      get_filename_component(dest_dir "${deploy_path}" DIRECTORY)
      install(
        FILES "${src_file}"
        DESTINATION "${module_dir}/${dest_dir}"
        RENAME "${dst_name}"
      )
    endforeach()
  endif()
endfunction()
