find_package(Qt6 REQUIRED COMPONENTS Core Quick Qml)

qt6_add_library(myshell_configs SHARED)

set_target_properties(myshell_configs PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(myshell_configs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(generated)

target_sources(myshell_configs PRIVATE config.cpp)

qt6_add_qml_module(myshell_configs URI "MyShellPlugin.Configs" VERSION 0.1)

qt6_query_qml_module(myshell_configs TARGET_PATH module_target_path)
set(MYSHELL_CONFIG_QML_TARGET_PATH "${module_target_path}" CACHE INTERNAL "")

target_link_libraries(
  myshell_configs
  PRIVATE
    Qt::Core
    Qt::Quick
    Qt::Qml
    myshell_include
    myshell_interfaces
    myshell_utils
    myshell_configs_gen
)

install_qml_module(myshell_configs)
