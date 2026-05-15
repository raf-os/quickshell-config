find_package(Qt6 REQUIRED COMPONENTS Core DBus)
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

add_library(myshell_interfaces
  STATIC
    ${NM_GENERATED_SOURCES}
    iconfigserializer.h
)

set_target_properties(myshell_interfaces PROPERTIES
  POSITION_INDEPENDENT_CODE ON)
target_include_directories(myshell_interfaces
  PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(myshell_interfaces PRIVATE Qt6::Core Qt6::DBus)
