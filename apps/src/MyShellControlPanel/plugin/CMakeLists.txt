find_package(Qt6 REQUIRED COMPONENTS Core Qml Quick)

add_subdirectory(validators)

cp_add_module(panel_plugin
  URI MyShellControlPanel.plugin
  SOURCES
    autoformfield.h autoformfield.cpp
    formcontroller.h formcontroller.cpp
    fieldcontroller.h fieldcontroller.cpp
    forminput.h forminput.cpp
  LIBRARIES
    Qt::Core
    Qt::Qml
    Qt::Quick
    myshell_configs
    myshell_interfaces
    panel_plugin_validators
)
