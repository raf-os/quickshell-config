find_package(
  Qt6
  REQUIRED
  COMPONENTS Core Gui Qml Quick Network
)

qt6_add_library(myshell_hyprland SHARED)

target_include_directories(myshell_hyprland PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(
  myshell_hyprland
  PRIVATE hyprland.cpp hyprevents.cpp hyprinputconfig.cpp hyprkeyboardlayout.cpp
)

qt6_add_qml_module(
  myshell_hyprland
  URI "${ORG_URI_PREFIX}.Hyprland"
  VERSION 0.1
)

target_link_libraries(
  myshell_hyprland
  PRIVATE Qt6::Core Qt6::Qml Qt6::Network myshell_utils
)

install_qml_module(myshell_hyprland)
