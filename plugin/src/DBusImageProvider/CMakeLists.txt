find_package(
  Qt6
  REQUIRED
  COMPONENTS Core DBus Gui Qml Quick
)

set(QT_QML_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml")

qt_add_library(nightshell_dbus_imageprovider SHARED dbusimage.cpp)

target_include_directories(
  nightshell_dbus_imageprovider
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

qt_add_qml_module(
  nightshell_dbus_imageprovider
  URI
  "${ORG_URI_PREFIX}.DBusImageProvider"
  VERSION
  1.0
  PLUGIN_TARGET
  nightshell_dbus_imageprovider
  NO_PLUGIN_OPTIONAL
  NO_GENERATE_PLUGIN_SOURCE
  CLASS_NAME
  DBusImageProviderPlugin
)

target_compile_definitions(
  nightshell_dbus_imageprovider
  PRIVATE DBUSPROVIDER_SHARED_LIB
)

set_target_properties(
  nightshell_dbus_imageprovider
  PROPERTIES POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(
  nightshell_dbus_imageprovider
  PRIVATE Qt::Core Qt::Gui Qt::Qml Qt::Quick Qt::DBus
)

install_qml_module(nightshell_dbus_imageprovider)
