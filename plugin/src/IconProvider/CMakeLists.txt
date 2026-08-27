find_package(
  Qt6
  REQUIRED
  COMPONENTS Core Qml Gui Quick Svg
)

set(QT_QML_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml")

qt6_add_library(nightshell_icon_provider_plugin SHARED iconprovider.cpp)

target_include_directories(
  nightshell_icon_provider_plugin
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(
  nightshell_icon_provider_plugin
  PUBLIC ICON_DEFAULT_PATH="${MYSHELL_INSTALL_DATADIR}"
  PRIVATE NS_ICONPROVIDER_SHARED_LIB
)

qt6_add_qml_module(
  nightshell_icon_provider_plugin
  URI "${ORG_URI_PREFIX}.IconProvider"
  VERSION 0.1
  PLUGIN_TARGET nightshell_icon_provider_plugin
  NO_PLUGIN_OPTIONAL
  NO_GENERATE_PLUGIN_SOURCE
  CLASS_NAME IconImageProviderExtensionPlugin
)

target_link_libraries(
  nightshell_icon_provider_plugin
  PRIVATE Qt6::Core Qt6::Qml Qt6::Gui Qt6::Quick Qt6::Svg
)

install_qml_module(nightshell_icon_provider_plugin)
