find_package(Qt6 REQUIRED COMPONENTS Core Qml)

qt6_add_library(nightshell_configs SHARED)

add_subdirectory(generated)

target_sources(nightshell_configs PRIVATE colors.cpp config.cpp)

set_target_properties(
  nightshell_configs
  PROPERTIES POSITION_INDEPENDENT_CODE ON
)
target_compile_definitions(nightshell_configs PRIVATE NS_CONFIGS_SHARED_LIB)

target_include_directories(
  nightshell_configs
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

set_source_files_properties(Styles.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt6_add_qml_module(
  nightshell_configs
  URI "${ORG_URI_PREFIX}.Configs"
  VERSION 0.1
  QML_FILES Styles.qml
  SOURCES ${GENERATED_SOURCES}
)

target_link_libraries(
  nightshell_configs
  PRIVATE
    Qt6::Core
    Qt6::Qml
    nightshell_includes
    nightshell_utils
    nightshell_configs_generated
)

install_qml_module(nightshell_configs)
