find_package(Qt6 REQUIRED COMPONENTS Core Xml Test)

qt6_add_executable(kbdlayout_test test_kbdlayoutparser.cpp)

target_compile_definitions(
  kbdlayout_test
  PRIVATE SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

add_test(NAME kbdlayout_test COMMAND kbdlayout_test)

target_link_libraries(
  kbdlayout_test
  PRIVATE Qt6::Core Qt6::Test nightshell_utils
)
