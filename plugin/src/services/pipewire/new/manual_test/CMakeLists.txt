find_package(Qt6 REQUIRED COMPONENTS Core Qml Quick)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/.qmlls.ini.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/.qmlls.ini"
)

qt_add_executable(nightshell_pipewire_manual_tests main.cpp)

set_target_properties(
  nightshell_pipewire_manual_tests
  PROPERTIES OUTPUT_NAME "pipewire-manual-test"
)

target_include_directories(
  nightshell_pipewire_manual_tests
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(
  nightshell_pipewire_manual_tests
  PRIVATE PLUGINS_DIR="${CMAKE_BINARY_DIR}/qml"
)

target_compile_options(
  nightshell_pipewire_manual_tests
  PRIVATE -fsanitize=address -fno-omit-frame-pointer -g
)
target_link_options(nightshell_pipewire_manual_tests PRIVATE -fsanitize=address)

qt6_add_qml_module(
  nightshell_pipewire_manual_tests
  URI "tests.pipewire_manual"
  QML_FILES Main.qml
)

target_link_libraries(
  nightshell_pipewire_manual_tests
  PRIVATE Qt6::Core Qt6::Qml Qt6::Quick nightshell_services_pipewire
)
