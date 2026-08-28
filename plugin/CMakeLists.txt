set(QT_QML_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml")

add_subdirectory(src/DBusImageProvider)
add_subdirectory(src/MyShellPlugin)
add_subdirectory(src/IconProvider)
add_subdirectory(src/DesktopEntries)
add_subdirectory(src/wayland)
add_subdirectory(src/dbus)
add_subdirectory(src/SystemTray)
