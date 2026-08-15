find_package(
  Qt6
  REQUIRED
  COMPONENTS Core Qml Gui GuiPrivate Quick QuickPrivate
)

set(QT_QML_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml")

find_package(PkgConfig REQUIRED)
pkg_check_modules(dmabuf-deps REQUIRED IMPORTED_TARGET libdrm gbm egl)

find_package(VulkanHeaders REQUIRED)

qt6_add_library(
  nightshell_wayland_buffer
  STATIC
  wlbuffer.cpp
  wlbufferrequest.cpp
  gbmdevice.cpp
  dmabuf.cpp
  dmabufutils.cpp
  linuxdmabufmanager.cpp
  manager.cpp
  qsg.cpp
  shm.cpp
  dmabuftex_opengl.cpp
  dmabuftex_vulkan.cpp
)

target_include_directories(
  nightshell_wayland_buffer
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

gen_wayland_protocol(
  wlp-linux-dmabuf
  linux-dmabuf-v1
  "${WAYLAND_PROTOCOLS}/stable/linux-dmabuf"
)

set_target_properties(wlp-linux-dmabuf PROPERTIES POSITION_INDEPENDENT_CODE ON)

set_target_properties(
  nightshell_wayland_buffer
  PROPERTIES POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(
  nightshell_wayland_buffer
  PRIVATE
    Qt6::Core
    Qt6::Quick
    Qt6::QuickPrivate
    Qt6::GuiPrivate
    Qt6::WaylandClient
    Qt6::WaylandClientPrivate
    PkgConfig::WAYLAND_CLIENT
    PkgConfig::dmabuf-deps
    wlp-linux-dmabuf
    Vulkan::Headers
    myshell_interfaces
)

add_subdirectory(wlr)
add_subdirectory(toplevels)
add_subdirectory(screencopy)

qt6_add_library(nightshell_wayland SHARED)

target_sources(nightshell_wayland PRIVATE screencopy/screencopy_qml_view.cpp)

target_include_directories(
  nightshell_wayland
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/screencopy"
)

qt6_add_qml_module(
  nightshell_wayland
  URI "${ORG_URI_PREFIX}.Wayland"
  VERSION 0.1
  DEPENDENCIES QtQml
)

target_compile_definitions(nightshell_wayland PRIVATE NS_WAYLAND_SHARED_LIB)

target_link_libraries(
  nightshell_wayland
  PRIVATE
    Qt6::Core
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickPrivate
    Qt6::GuiPrivate
    Qt6::WaylandClient
    Qt6::WaylandClientPrivate
  PUBLIC
    nightshell_wayland_screencopy
    nightshell_wayland_buffer
    nightshell_wayland_toplevels
    nightshell_wayland_wlr
    myshell_hyprland
)

set_target_properties(
  nightshell_wayland
  PROPERTIES POSITION_INDEPENDENT_CODE ON
)

install_qml_module(nightshell_wayland)
