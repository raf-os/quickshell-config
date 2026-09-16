find_package(Qt6 REQUIRED COMPONENTS Core Qml)

qt6_add_library(nightshell_utils SHARED)

target_sources(nightshell_utils PRIVATE paths.cpp serializableobject.cpp)

set_target_properties(nightshell_utils PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(
  nightshell_utils
  PRIVATE NS_UTILS_SHARED_LIB
  PUBLIC DATADIR_DEFAULT_PATH="${MYSHELL_INSTALL_DATADIR}"
)

target_include_directories(nightshell_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

qt6_add_qml_module(nightshell_utils URI "${ORG_URI_PREFIX}.Utils" VERSION 0.1)

target_link_libraries(nightshell_utils PRIVATE Qt6::Core Qt6::Qml)

install_qml_module(nightshell_utils)
