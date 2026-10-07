find_package(Qt6 REQUIRED COMPONENTS Core Qml)
find_library(PAM NAMES pam REQUIRED)

qt6_add_library(nightshell_services_pam STATIC)

target_sources(
  nightshell_services_pam
  PRIVATE conversation.cpp process.cpp ptypes.cpp
)

set_target_properties(
  nightshell_services_pam
  PROPERTIES POSITION_INDEPENDENT_CODE ON
)

target_include_directories(
  nightshell_services_pam
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

qt6_add_qml_module(
  nightshell_services_pam
  URI "${ORG_URI_PREFIX}.Services.Pam"
  VERSION 0.1
)

set_target_properties(
  nightshell_services_pamplugin
  PROPERTIES POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(nightshell_services_pam PRIVATE Qt6::Core Qt6::Qml pam)

install_qml_module(nightshell_services_pam)
