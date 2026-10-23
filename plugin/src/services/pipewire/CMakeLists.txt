find_package(Qt6 REQUIRED COMPONENTS Core Qml Quick)

find_package(PkgConfig REQUIRED)
pkg_check_modules(pipewire REQUIRED IMPORTED_TARGET libpipewire-0.3)

qt6_add_library(nightshell_services_pipewire SHARED)

qt_declare_logging_category(
  nightshell_services_pipewire
  IDENTIFIER
    "ns::services::pipewire::NS_LOG_PIPEWIRE"
  HEADER
    "ns_services_pipewire_debug.h"
  CATEGORY_NAME
    "ns.services.pipewire"
  DEFAULT_SEVERITY
    Debug
)

qt_generate_export_macros(
  nightshell_services_pipewire
  HEADER "ns_services_pipewire_export.h"
  NAME "NS_SERVICES_PIPEWIRE"
)

target_sources(
  nightshell_services_pipewire
  PRIVATE
    pwcore.cpp
    pw_spa_hook.cpp
    pw_metadata.cpp
    pw_device.cpp
    pw_common.cpp
    pw_node.cpp
    pw_node_bound_audio.cpp
    pw_connection.cpp
    pw_defaults.cpp
    pw_link.cpp
    pwregistry.cpp
    qml/qml_pipewire.cpp
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
