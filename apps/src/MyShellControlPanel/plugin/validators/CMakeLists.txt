find_package(Qt6 REQUIRED COMPONENTS Core Qml Quick)

cp_add_module(panel_plugin_validators
  URI MyShellControlPanel.plugin.validators
  SOURCES
    validatorresponse.h
    validator.h validator.cpp
    fontvalidator.h fontvalidator.cpp
  LIBRARIES
    Qt::Core
    Qt::Qml
    Qt::Quick
    myshell_utils
    myshell_interfaces
)

target_include_directories(
  panel_plugin_validators
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
