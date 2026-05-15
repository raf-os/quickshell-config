find_package(Qt6 REQUIRED COMPONENTS Core Qml Quick)

cp_add_module(panel_plugin
  URI MyShellControlPanel.plugin
  SOURCES
    formcontroller.h formcontroller.cpp
    fieldcontroller.h fieldcontroller.cpp
  LIBRARIES
    Qt::Core
    Qt::Qml
    Qt::Quick
    myshell_configs
    myshell_interfaces
)
