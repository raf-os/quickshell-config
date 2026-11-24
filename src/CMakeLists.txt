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
)

if(DEVMODE)
  target_compile_definitions(
    nightshell
    PRIVATE QT_QML_DEBUG QML_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/qml"
  )

  file(GLOB_RECURSE QML_MODULE_QRCS "${CMAKE_BINARY_DIR}/qml/ns/*.qrc")
  set(QMLPREVIEW_ARGS "")
  foreach(qrc IN LISTS QML_MODULE_QRCS)
    list(APPEND QMLPREVIEW_ARGS --resource "\"${qrc}\"")
  endforeach()

  add_custom_target(
    nightshell_qmlpreview
    COMMAND qmlpreview6 ${QMLPREVIEW_ARGS} $<TARGET_FILE:nightshell>
    DEPENDS nightshell
    USES_TERMINAL
  )
endif()
