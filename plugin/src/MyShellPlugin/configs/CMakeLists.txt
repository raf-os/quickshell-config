find_package(Qt6 REQUIRED COMPONENTS Core Quick Qml)

add_library(myshell_configs
  STATIC
    config.h config.cpp)

set_target_properties(myshell_configs PROPERTIES
  POSITION_INDEPENDENT_CODE ON)

target_include_directories(myshell_configs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(generated)

target_link_libraries(myshell_configs
  PRIVATE
    Qt::Core
    Qt::Quick
    Qt::Qml
    myshell_include
    myshell_utils
    myshell_configs_gen)

qml_module(myshell_configs_plugin
  URI MyShellPlugin.Configs
  SOURCES
    config.h config.cpp
    ${GENERATED_SOURCES}
  LIBRARIES
    Qt::Core
    Qt::Quick
    Qt::Qml
    myshell_include
    myshell_utils
    myshell_configs_gen)
