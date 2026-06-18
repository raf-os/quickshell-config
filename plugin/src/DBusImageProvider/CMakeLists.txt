find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)

set(QT_QML_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml")

qt_add_library(nightshell_dbus_imageprovider SHARED dbusimage.cpp)

target_include_directories(nightshell_dbus_imageprovider
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

qt_add_qml_module(nightshell_dbus_imageprovider
  URI "${ORG_URI_PREFIX}.DBusImageProvider"
  VERSION 1.0
  PLUGIN_TARGET nightshell_dbus_imageprovider
  NO_PLUGIN_OPTIONAL
  NO_GENERATE_PLUGIN_SOURCE
  CLASS_NAME DBusImageProviderPlugin
)

target_link_libraries(nightshell_dbus_imageprovider PRIVATE
  Qt::Core
  Qt::Gui
  Qt::Qml
  Qt::Quick)

install_qml_module(nightshell_dbus_imageprovider)
