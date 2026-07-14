cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED VERSION)
  execute_process(
    COMMAND git describe --tags --abbrev=0
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  if("${VERSION}" STREQUAL "")
    message(FATAL_ERROR "Failed to get VERSION from git")
  endif()
endif()

string(REGEX REPLACE "^v" "" VERSION "${VERSION}")

project(myshell VERSION ${VERSION} LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

set(QT_NO_PRIVATE_MODULE_WARNING ON)

set(INSTALL_QMLDIR "/usr/lib/qt6/qml")
set(ORG_URI_PREFIX "org.nightshell")
set(INSTALL_QSCONFDIR "/etc/xdg/quickshell")

set(MYSHELL_QML_TARGET_PATH "" CACHE INTERNAL "")
set(MYSHELLUTILS_QML_TARGET_PATH "" CACHE INTERNAL "")
set(MYSHELL_CONFIG_QML_TARGET_PATH "" CACHE INTERNAL "")
set(MYSHELL_CONTROL_PANEL_LIBPATH "" CACHE INTERNAL "")

include(FetchContent)

include(GNUInstallDirs)

include(cmake/install-qml-module.cmake)
include(cmake/gen-wayland-protocol.cmake)

if(DEVMODE)
  set(MYSHELL_INSTALL_DATADIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")
else()
  set(
    MYSHELL_INSTALL_DATADIR
    "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/nightshell"
  )
endif()

# This is simply a hack, there has to be a better way of doing this
list(
  APPEND CMAKE_INSTALL_RPATH
  "${MYSHELL_QML_TARGET_PATH}"
  "${INSTALL_QMLDIR}/MyShellPlugin/Configs"
  "${INSTALL_QMLDIR}/MyShellPlugin/Utils"
  "${INSTALL_QMLDIR}/org/nightshell/Wallpaper"
  "${INSTALL_QMLDIR}/org/nightshell/DesktopEntries"
  "${INSTALL_QMLDIR}/org/nightshell/Hyprland"
  "${INSTALL_QMLDIR}/org/nightshell/Hyprland/Toplevels"
)

find_package(Qt6 REQUIRED COMPONENTS Core)

qt_standard_project_setup(REQUIRES 6.9)

install(
  DIRECTORY "${CMAKE_SOURCE_DIR}/myshell"
  DESTINATION "${INSTALL_QSCONFDIR}"
)

install(FILES LICENSE DESTINATION "${INSTALL_QSCONFDIR}/myshell")

add_subdirectory(plugin)
add_subdirectory(apps)
add_subdirectory(assets)

add_subdirectory(ipc)
