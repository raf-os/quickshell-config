qt6_add_library(ns_interfaces STATIC)

target_sources(ns_interfaces PRIVATE)

target_include_directories(ns_interfaces PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(ns_interfaces PRIVATE Qt6::Core)
