find_package(Qt6 REQUIRED COMPONENTS Core Gui DBus)

qt6_add_executable(ns_notification_tester main.cpp)

target_compile_definitions(
  ns_notification_tester
  PRIVATE ICON_TEST_PATH="${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(
  ns_notification_tester
  PRIVATE Qt6::Core Qt6::DBus Qt6::Gui ns_notification_client
)
