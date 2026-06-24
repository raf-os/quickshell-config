find_package(Qt6 REQUIRED COMPONENTS Core Qml DBus)

qt_add_library(nightshell_ipc_server SHARED dbusserver.cpp)

target_include_directories(nightshell_ipc_server
                           PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

qt_add_qml_module(nightshell_ipc_server URI "${ORG_URI_PREFIX}.IpcServer"
                  VERSION 0.1)

target_link_libraries(nightshell_ipc_server PRIVATE Qt::Core Qt::Qml Qt::DBus)

install_qml_module(nighshell_ipc_server)
