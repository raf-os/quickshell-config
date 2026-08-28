find_package(Qt6 REQUIRED COMPONENTS Core DBus)

set_source_files_properties(
  com.canonical.dbusmenu.xml
  PROPERTIES
    CLASSNAME DBusMenuInterface
    INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/dbusmenu_types.h
)

qt6_add_dbus_interface(DBUS_INTERFACES com.canonical.dbusmenu.xml dbus_menu)

qt6_add_library(nightshell_dbusmenu SHARED)

target_sources(
  nightshell_dbusmenu
  PRIVATE dbusmenu_types.cpp dbusmenu.cpp dbusmenuitem.cpp ${DBUS_INTERFACES}
)

target_include_directories(
  nightshell_dbusmenu
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
)

set_target_properties(
  nightshell_dbusmenu
  PROPERTIES POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(
  nightshell_dbusmenu
  PRIVATE Qt6::Core Qt6::DBus nightshell_dbus
)
