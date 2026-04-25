cmake_minimum_required(VERSION 3.16)

if (NOT DEFINED VERSION)
  execute_process(COMMAND git describe --tags --abbrev=0
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  if ("${VERSION}" STREQUAL "")
    message(FATAL_ERROR "Failed to get VERSION from git")
  endif()
endif()

string(REGEX REPLACE "^v" "" VERSION "${VERSION}")

project(myshell
  VERSION ${VERSION}
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

set(INSTALL_QMLDIR "/usr/lib/qt6/qml")
set(INSTALL_QSCONFDIR "/etc/xdg/quickshell")

set(MYSHELL_QML_TARGET_PATH "" CACHE INTERNAL "")
set(MYSHELLUTILS_QML_TARGET_PATH "" CACHE INTERNAL "")

include(FetchContent)

find_package(Qt6 REQUIRED COMPONENTS Core)

qt_standard_project_setup(REQUIRES 6.9)

install(DIRECTORY "${CMAKE_SOURCE_DIR}/myshell"
  DESTINATION "${INSTALL_QSCONFDIR}")

install(FILES LICENSE DESTINATION "${INSTALL_QSCONFDIR}/myshell")

add_subdirectory(plugin)
add_subdirectory(apps)
