find_package(Qt6 REQUIRED COMPONENTS Core Xml Test)

qt6_add_executable(hyprconfigsparser_test test_hyprconfigsparser.cpp)

target_compile_definitions(
  hyprconfigsparser_test
  PRIVATE TEST_DATA_DIR="${CMAKE_SOURCE_DIR}/tests/_data"
)

add_test(NAME hyprconfigsparser_test COMMAND hyprconfigsparser_test)

target_link_libraries(
  hyprconfigsparser_test
  PRIVATE Qt6::Core Qt6::Test nightshell_utils ns_wl_hyprland
)
