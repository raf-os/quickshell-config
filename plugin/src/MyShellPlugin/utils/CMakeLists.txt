find_package(Qt6 REQUIRED COMPONENTS Core Quick Qml)

add_library(myshell_utils
  STATIC
    metaiterate.h metaiterate.cpp
    paths.h paths.cpp)

set_target_properties(myshell_utils PROPERTIES
  POSITION_INDEPENDENT_CODE ON)

target_include_directories(myshell_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(myshell_utils
  PRIVATE
    Qt::Core
    Qt::Quick
    Qt::Qml
    myshell_include)

qml_module(myshell_utils_plugin
  URI MyShellPlugin.Utils
  SOURCES
    paths.h paths.cpp
  LIBRARIES
    Qt::Core
    Qt::Quick
    Qt::Qml
    myshell_include
    myshell_utils
)
