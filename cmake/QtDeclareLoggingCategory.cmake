# Based off of
# https://github.com/KDE/extra-cmake-modules/blob/fe0f606bf7f222e36f7560fd7a2c33ef993e23bb/modules/ECMQtDeclareLoggingCategory.cmake

set(
  _QT_DECLARE_LOGGING_CATEGORY_TEMPLATE_CPP
  "${CMAKE_CURRENT_LIST_DIR}/QtDeclareLoggingCategory.cpp.in"
)
set(
  _QT_DECLARE_LOGGING_CATEGORY_TEMPLATE_H
  "${CMAKE_CURRENT_LIST_DIR}/QtDeclareLoggingCategory.h.in"
)

function(qt_declare_logging_category sources_var)
  set(options)
  set(
    oneValueArgs
    HEADER
    IDENTIFIER
    CATEGORY_NAME
    DEFAULT_SEVERITY
    DESCRIPTION
  )
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
      "Unexpected arguments to qt_declare_logging_category: ${ARG_UNPARSED_ARGUMENTS}"
    )
  endif()
  if(NOT ARG_HEADER)
    message(
      FATAL_ERROR
      "Missing HEADER argument for qt_declare_logging_category"
    )
  endif()
  if(NOT ARG_IDENTIFIER)
    message(
      FATAL_ERROR
      "Missing IDENTIFIER argument for qt_declare_logging_category"
    )
  endif()
  if(NOT ARG_CATEGORY_NAME)
    message(
      FATAL_ERROR
      "Missing CATEGORY_NAME argument for qt_declare_logging_category"
    )
  endif()
  if(NOT ARG_DEFAULT_SEVERITY)
    set(ARG_DEFAULT_SEVERITY Info)
    set(is_explicit_default_severity FALSE)
  else()
    set(
      acceptable_severities
      Debug
      Info
      Warning
      Critical
      Fatal
    )
    list(FIND acceptable_severities "${ARG_DEFAULT_SEVERITY}" pos)
    if(pos EQUAL -1)
      message(FATAL_ERROR "Unknown severity ${pos}")
    endif()
    set(is_explicit_default_severity TRUE)
  endif()

  if(TARGET ${sources_var})
    get_target_property(aliased_target ${sources_var} ALIASED_TARGET)
    if(aliased_target)
      message(
        FATAL_ERROR
        "Target passed to qt_declare_logging_category must not be an alias: ${sources_var}"
      )
    endif()
  endif()

  if(NOT IS_ABSOLUTE "${ARG_HEADER}")
    set(ARG_HEADER "${CMAKE_CURRENT_BINARY_DIR}/${ARG_HEADER}")
  endif()

  string(REPLACE "::" ";" namespaces "${ARG_IDENTIFIER}")
  list(LENGTH namespaces len)
  math(EXPR last_pos "${len} - 1")
  list(GET namespaces ${last_pos} IDENTIFIER)
  list(REMOVE_AT namespaces ${last_pos})

  set(OPEN_NAMESPACES)
  set(CLOSE_NAMESPACES)
  foreach(ns ${namespaces})
    set(OPEN_NAMESPACES "${OPEN_NAMESPACES}\nnamespace ${ns} {")
    set(CLOSE_NAMESPACES "} // namespace ${ns}\n${CLOSE_NAMESPACES}")
  endforeach()

  string(FIND "${ARG_HEADER}" "." pos REVERSE)
  if(pos EQUAL -1)
    set(cpp_filename "${ARG_HEADER}.cpp")
  else()
    string(SUBSTRING "${ARG_HEADER}" 0 ${pos} cpp_filename)
    set(cpp_filename "${cpp_filename}.cpp")
  endif()

  get_filename_component(HEADER_NAME "${ARG_HEADER}" NAME)

  if(NOT _QT_DECLARE_LOGGING_CATEGORY_TEMPLATE_CPP)
    message(
      FATAL_ERROR
      "You must include(QtDeclareLoggingCategory) before using qt_declare_logging_category"
    )
  endif()

  configure_file(
    "${_QT_DECLARE_LOGGING_CATEGORY_TEMPLATE_CPP}"
    "${cpp_filename}"
  )
  configure_file("${_QT_DECLARE_LOGGING_CATEGORY_TEMPLATE_H}" "${ARG_HEADER}")

  if(TARGET ${sources_var})
    target_sources(${sources_var} PRIVATE ${cpp_filename} "${ARG_HEADER}")
  else()
    set(sources "${${sources_var}}")
    list(APPEND sources "${cpp_filename}")
    set(${sources_var} "${sources}" PARENT_SCOPE)
  endif()
endfunction()
