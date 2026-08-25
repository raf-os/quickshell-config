find_package(Qt6 REQUIRED COMPONENTS Core DBus)

qt6_add_library(nightshell_dbus STATIC)

target_sources(nightshell_dbus PRIVATE dbusutils.cpp dbuspropertygroup.cpp)

target_include_directories(nightshell_dbus PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(nightshell_dbus PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_link_libraries(nightshell_dbus PRIVATE Qt6::Core Qt6::DBus)
