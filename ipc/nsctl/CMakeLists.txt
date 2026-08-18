find_package(Qt6 REQUIRED COMPONENTS Core DBus)

qt_add_executable(nightshell_ctl main.cpp)

target_sources(nightshell_ctl PRIVATE wallpaper.cpp misc.cpp dbus.cpp)

target_include_directories(nightshell_ctl PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

set_target_properties(
  nightshell_ctl
  PROPERTIES
    OUTPUT_NAME "nsctl"
    INSTALL_RPATH "${INSTALL_QMLDIR}/org/nightshell/IpcServer"
)

target_link_libraries(
  nightshell_ctl
  PRIVATE Qt::Core Qt::DBus nightshell_ipc_client
)

include(GNUInstallDirs)

install(
  TARGETS nightshell_ctl
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
