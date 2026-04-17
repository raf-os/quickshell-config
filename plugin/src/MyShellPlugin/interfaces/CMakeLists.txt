find_package(Qt6 REQUIRED COMPONENTS DBus)
find_program(XMLLINT xmllint)

if(XMLLINT)
  execute_process(
    COMMAND
      ${XMLLINT}
      --noout
      ${CMAKE_CURRENT_SOURCE_DIR}/org.freedesktop.NetworkManager.xml
    RESULT_VARIABLE XMLLINT_RESULT
  )
  if(NOT XMLLINT_RESULT EQUAL 0)
    message(FATAL_ERROR "NetworkManager D-Bus XML is malformed")
  endif()
endif()

qt_add_dbus_interface(NM_GENERATED_SOURCES
  org.freedesktop.NetworkManager.xml
  nm_interface)

add_library(nm_dbus STATIC ${NM_GENERATED_SOURCES})

set_target_properties(nm_dbus PROPERTIES
  POSITION_INDEPENDENT_CODE ON)
target_include_directories(nm_dbus PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(nm_dbus PRIVATE Qt6::DBus)
