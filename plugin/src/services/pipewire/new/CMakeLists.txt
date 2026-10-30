find_package(Qt6 REQUIRED COMPONENTS Core Qml Quick)

find_package(PkgConfig REQUIRED)
pkg_check_modules(pipewire REQUIRED IMPORTED_TARGET libpipewire-0.3)

qt6_add_library(nightshell_services_pipewire SHARED)

qt_declare_logging_category(
  nightshell_services_pipewire
  IDENTIFIER
    "ns::pipewire::NS_LOG_PIPEWIRE"
  HEADER
    "ns_pipewire_debug.h"
  CATEGORY_NAME
    "ns.pipewire"
  DEFAULT_SEVERITY
    Debug
)

qt_generate_export_macros(
  nightshell_services_pipewire
  HEADER "ns_pipewire_export.h"
  NAME "NS_PIPEWIRE"
)

target_sources(
  nightshell_services_pipewire
  PRIVATE
    pw_globals.cpp
    pw_core.cpp
    pw_registry.cpp
    pw_spa_hook.cpp
    pw_trackable.cpp
    pw_device.cpp
    pw_node.cpp
    pw_node_data.cpp
    pw_defaults.cpp
    pw_link.cpp
    pw_metadata.cpp
    pw_common.cpp
    qml/qml_object_interface.cpp
    qml/qml_pipewire.cpp
    qml/qml_objects.cpp
)

target_include_directories(
  nightshell_services_pipewire
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/qml
)

set_target_properties(
  nightshell_services_pipewire
  PROPERTIES POSITION_INDEPENDENT_CODE ON
)

qt6_add_qml_module(
  nightshell_services_pipewire
  URI "${ORG_URI_PREFIX}.Services.Pipewire"
  VERSION 0.1
)

target_link_libraries(
  nightshell_services_pipewire
  PRIVATE Qt6::Core Qt6::Qml Qt6::Quick PkgConfig::pipewire nightshell_includes
)

install_qml_module(nightshell_services_pipewire)

add_subdirectory(manual_test)
