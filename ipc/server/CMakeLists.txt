find_package(Qt6 REQUIRED COMPONENTS Core Qml DBus)

set(QT_QML_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml")

qt_add_dbus_adaptor(
  IPC_DBUS_ADAPTOR org.nightshell.socket.xml dbusserver.h
  ns::ipc::server::IPCServer ns_ipc_server NightshellIpcServer
)

qt_add_dbus_interface(IPC_DBUS_INTERFACE org.nightshell.socket.xml
                      ns_ipc_client
)

qt_add_library(nightshell_ipc_server SHARED dbusserver.cpp ${IPC_DBUS_ADAPTOR})

qt_add_library(nightshell_ipc_client STATIC ${IPC_DBUS_INTERFACE})

target_include_directories(
  nightshell_ipc_server
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(
  nightshell_ipc_client
  PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
)

qt_add_qml_module(
  nightshell_ipc_server
  URI
  "${ORG_URI_PREFIX}.IpcServer"
  VERSION
  0.1
)

target_link_libraries(
  nightshell_ipc_server
  PRIVATE Qt::Core Qt::Qml Qt::DBus nightshell_wallpaper
)

target_link_libraries(nightshell_ipc_client PRIVATE Qt::Core Qt::DBus)

install_qml_module(nightshell_ipc_server)
