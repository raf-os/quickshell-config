find_package(Qt6 REQUIRED COMPONENTS Core Quick)

qml_module(myshell_configs_plugin
  URI MyShellPlugin.Configs
  SOURCES
    config.h config.cpp
  LIBRARIES
    Qt::Core
    Qt::Quick
    myshell_include)

# set_target_properties(myshell_configs PROPERTIES
#   POSITION_INDEPENDENT_CODE ON)
#
# target_include_directories(myshell_configs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
