find_package(
  Qt6
  REQUIRED
  COMPONENTS Core Gui Qml Quick Network
)

qt6_add_library(myshell_hyprland SHARED)

target_include_directories(myshell_hyprland PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(
  myshell_hyprland
  PRIVATE
    hyprland.cpp
    hyprevents.cpp
    hyprinputconfig.cpp
    hyprkeyboardlayout.cpp
    hyprworkspace.cpp
    toplevelmodel.cpp
    workspacesmodel.cpp
)

set_target_properties(myshell_hyprland PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_subdirectory(toplevels)
add_subdirectory(common)

qt6_add_qml_module(
  myshell_hyprland
  URI "${ORG_URI_PREFIX}.Hyprland"
  VERSION 0.1
)

target_link_libraries(
  myshell_hyprland
  PRIVATE
    Qt6::Core
    Qt6::Quick
    Qt6::Qml
    Qt6::Network
    rapidfuzz::rapidfuzz
    myshell_utils
    myshell_include
  PUBLIC myshell_hyprland_toplevels nightshell_hyprland_common
)

install_qml_module(myshell_hyprland)
