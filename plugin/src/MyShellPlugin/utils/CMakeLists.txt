find_package(Qt6 REQUIRED COMPONENTS Core)

add_library(myshell_utils
  STATIC
    metaiterate.h metaiterate.cpp)

set_target_properties(myshell_utils PROPERTIES
  POSITION_INDEPENDENT_CODE ON)

target_include_directories(myshell_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(myshell_utils
  PUBLIC
   Qt6::Core)
