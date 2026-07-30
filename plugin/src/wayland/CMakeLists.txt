find_package(
  Qt6
  REQUIRED
  COMPONENTS Core Qml Gui GuiPrivate Quick QuickPrivate
)

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
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

gen_wayland_protocol(
  wlp-linux-dmabuf
  linux-dmabuf-v1
  "${WAYLAND_PROTOCOLS}/stable/linux-dmabuf"
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
