find_package(Qt6 REQUIRED COMPONENTS Core Qml DBus)
find_program(XMLLINT xmllint)

if(XMLLINT)
  execute_process(
    COMMAND
      ${XMLLINT} --noout
      ${CMAKE_CURRENT_SOURCE_DIR}/org.freedesktop.NetworkManager.xml
    RESULT_VARIABLE XMLLINT_RESULT
  )
  if(NOT XMLLINT_RESULT EQUAL 0)
    message(FATAL_ERROR "NetworkManager D-Bus XML is malformed")
  endif()
endif()

qt_add_dbus_interface(NM_GENERATED_SOURCES
  org.freedesktop.NetworkManager.xml
  nm_interface
)

qt6_add_library(nightshell_services SHARED)

target_sources(
  nightshell_services
  PRIVATE systemtime.cpp networkservice.cpp ${NM_GENERATED_SOURCES}
)

set_target_properties(
  nightshell_services
  PROPERTIES POSITION_INDEPENDENT_CODE ON
)
target_compile_definitions(nightshell_services PRIVATE NS_SERVICES_SHARED_LIB)

set_source_files_properties(Time.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

target_include_directories(
  nightshell_services
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
)

qt6_add_qml_module(
  nightshell_services
  URI "${ORG_URI_PREFIX}.Services"
  VERSION 0.1
  QML_FILES Time.qml
)

target_link_libraries(
  nightshell_services
  PRIVATE Qt6::Core Qt6::Qml Qt6::DBus nightshell_includes
)

install_qml_module(nightshell_services)
