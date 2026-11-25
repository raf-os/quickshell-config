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
    Qt6::Quick
    ns_core
    ns_core_plugin
    ns_shell_root
    ns_shell_rootplugin
    nightshell_wayland
    nightshell_wayland_common
)

if(DEVMODE)
  target_compile_definitions(nightshell PRIVATE QT_QML_DEBUG)

  qt6_add_qml_preview(nightshell)
endif()
