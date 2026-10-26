set(
  _QT_GENERATE_EXPORT_MACROS_TEMPLATE_H
  "${CMAKE_CURRENT_LIST_DIR}/QtGenerateExportMacros.h.in"
)

function(qt_generate_export_macros arg_target)
  set(options)
  set(oneValueArgs HEADER NAME)
  set(multiValueArgs)
  cmake_parse_arguments(
    ARG
    "${options}"
    "${oneValueArgs}"
    "${multiValueArgs}"
    ${ARGN}
  )

  if(ARG_UNPARSED_ARGUMENTS)
    message(
      FATAL_ERROR
      "Unexpected arguments to qt_generate_export_macros: "
      "${ARG_UNPARSED_ARGUMENTS}"
    )
  endif()

  if(NOT ARG_HEADER)
    message(FATAL_ERROR "Missing HEADER argument for qt_generate_export_macros")
  endif()
  if(NOT ARG_NAME)
    message(FATAL_ERROR "Missing NAME argument for qt_generate_export_macros")
  endif()

  if(NOT IS_ABSOLUTE "${ARG_HEADER}")
    set(ARG_HEADER "${CMAKE_CURRENT_BINARY_DIR}/${ARG_HEADER}")
  endif()

  set(EXPORT_MACRO "${ARG_NAME}_EXPORT")
  set(NOEXPORT_MACRO "${ARG_NAME}_PRIVATE")
  set(SHARED_LIB_MACRO "${ARG_NAME}_SHARED_LIB")

  if(NOT _QT_GENERATE_EXPORT_MACROS_TEMPLATE_H)
    message(
      FATAL_ERROR
      "You must include(QtGenerateExportMacros) before using qt_generate_export_macros"
    )
  endif()

  configure_file("${_QT_GENERATE_EXPORT_MACROS_TEMPLATE_H}" "${ARG_HEADER}")

  target_include_directories(
    ${arg_target}
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
    INTERFACE ${CMAKE_CURRENT_BINARY_DIR}
  )
  target_compile_definitions(${arg_target} PRIVATE ${SHARED_LIB_MACRO})
endfunction()
