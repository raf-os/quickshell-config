find_package(Qt6 REQUIRED COMPONENTS Core DBus)

qt6_add_library(nightshell_dbus STATIC)

set_source_files_properties(
  org.freedesktop.DBus.Properties.xml
  PROPERTIES CLASSNAME DBusIntrospectionProperties
)
qt6_add_dbus_interface(
  DBUS_INTROSPECTION_INTERFACES
  org.freedesktop.DBus.Properties.xml
  dbus_introspection_properties
)

target_include_directories(
  nightshell_dbus
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
)

target_sources(
  nightshell_dbus
  PRIVATE dbusutils.cpp dbus_bindable_group.cpp ${DBUS_INTROSPECTION_INTERFACES}
)

qt_declare_logging_category(
  nightshell_dbus
  IDENTIFIER "ns::dbus::NS_LOG_DBUS"
  HEADER "ns_dbus_debug.h"
  CATEGORY_NAME "ns.dbus"
  DEFAULT_SEVERITY Debug
)

qt_generate_export_macros(
  nightshell_dbus
  HEADER "ns_dbus_export.h"
  NAME "NS_DBUS"
)

set_target_properties(nightshell_dbus PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_link_libraries(
  nightshell_dbus
  PRIVATE Qt6::Core Qt6::DBus nightshell_includes
)

add_subdirectory(dbusmenu)
