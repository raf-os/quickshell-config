find_package(Qt6 REQUIRED COMPONENTS Core DBus)

qt_add_executable(nightshell_ctl main.cpp)

set_target_properties(
  nightshell_ctl
  PROPERTIES OUTPUT_NAME "nsctl" INSTALL_RPATH
                                 "${INSTALL_QMLDIR}/org/nightshell/IpcServer")

target_link_libraries(nightshell_ctl PRIVATE Qt::Core Qt::DBus)

include(GNUInstallDirs)

install(
  TARGETS nightshell_ctl
          BUNDLE_DESTINATION
          .
          LIBRARY_DESTINATION
          ${CMAKE_INSTALL_LIBDIR}
          RUNTIME_DESTINATION
          ${CMAKE_INSTALL_BINDIR})
