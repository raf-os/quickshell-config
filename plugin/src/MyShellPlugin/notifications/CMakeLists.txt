find_package(Qt6 REQUIRED COMPONENTS
  Core
  Gui
  Qml
  Quick
  DBus
)

qt_add_dbus_adaptor(DBUS_INTERFACES
  org.freedesktop.Notifications.xml
  notificationserver.h
  ns::notifications::NotificationServer
  dbus_notifications
  DBusNotificationServer
)

qt_add_library(myshell_notifications SHARED
  notificationserver.cpp
  # notification.cpp
  ${DBUS_INTERFACES}
)

target_include_directories(
  myshell_notifications
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
)

qt_add_qml_module(
  myshell_notifications
  URI "${ORG_URI_PREFIX}.Notifications"
  VERSION 0.1
)

target_link_libraries(myshell_notifications
  PRIVATE
    Qt::Quick
    Qt::DBus
)

install_qml_module(myshell_notifications)
