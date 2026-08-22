find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml DBus)

qt6_add_dbus_adaptor(DBUS_INTERFACES
  org.kde.StatusNotifierWatcher.xml
  statusnotifierwatcher.h
  ns::systemtray::StatusNotifierWatcher
  dbus_watcher
  StatusNotifierWatcherAdaptor
)

set_source_files_properties(
  org.kde.StatusNotifierItem.xml
  PROPERTIES
    CLASSNAME QDBusStatusNotifierItem
    INCLUDE dbustypes.h # haha D bust types
)

qt6_add_dbus_interface(DBUS_INTERFACES org.kde.StatusNotifierItem.xml dbus_item)

set_source_files_properties(
  org.kde.StatusNotifierWatcher.xml
  PROPERTIES CLASSNAME QDBusStatusNotifierWatcher
)

qt6_add_dbus_interface(DBUS_INTERFACES
  org.kde.StatusNotifierWatcher.xml
  dbus_watcher_interface
)

qt6_add_library(nightshell_systemtray SHARED)

target_sources(
  nightshell_systemtray
  PRIVATE
    dbustypes.cpp
    statusnotifierhost.cpp
    statusnotifierwatcher.cpp
    statusnotifieritem.cpp
    ${DBUS_INTERFACES}
)

target_include_directories(
  nightshell_systemtray
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
  nightshell_systemtray
  PRIVATE Qt6::Core Qt6::Gui Qt6::Qml Qt6::DBus nightshell_dbus
)
