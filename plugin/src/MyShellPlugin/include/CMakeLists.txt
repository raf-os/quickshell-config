find_package(Qt6 REQUIRED COMPONENTS Core)

add_library(myshell_include INTERFACE)

target_link_libraries(myshell_include INTERFACE Qt::Core)

target_include_directories(myshell_include
  INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR})
