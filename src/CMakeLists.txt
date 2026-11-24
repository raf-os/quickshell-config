find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)

set(QT_QML_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml")

qt6_add_executable(nightshell main.cpp)

add_subdirectory(qml)

target_link_libraries(
  nightshell
  PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    ns_core
    ns_core_plugin
    ns_shell_root
    ns_shell_rootplugin
    nightshell_wayland
)
