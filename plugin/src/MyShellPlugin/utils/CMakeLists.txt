find_package(Qt6 REQUIRED COMPONENTS Core Quick Qml Xml)

qt6_add_library(myshell_utils SHARED)

target_sources(
  myshell_utils
  PRIVATE
    metaiterate.cpp
    paths.cpp
    cserializable.cpp
    soundthemes.cpp
    kbdlayouts.cpp
    objectmodel.cpp
)

# set_target_properties(myshell_utils PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_compile_definitions(
  myshell_utils
  PUBLIC DATADIR_DEFAULT_PATH="${MYSHELL_INSTALL_DATADIR}"
)

target_include_directories(myshell_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

qt6_add_qml_module(myshell_utils URI "MyShellPlugin.Utils" VERSION 0.1)

target_link_libraries(
  myshell_utils
  PRIVATE Qt::Core Qt::Quick Qt::Qml Qt::Xml myshell_include
)

install_qml_module(myshell_utils)

target_compile_definitions(
  myshell_utils
  PRIVATE DATADIR_DEFAULT_PATH="${MYSHELL_INSTALL_DATADIR}"
)
