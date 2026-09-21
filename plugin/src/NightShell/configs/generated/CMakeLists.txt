find_package(Qt6 REQUIRED COMPONENTS Core Qml Gui)

qt6_add_library(nightshell_configs_generated STATIC)

target_sources(nightshell_configs_generated PRIVATE
	appearance.cpp
	bar.cpp
)

target_include_directories(nightshell_configs_generated PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(nightshell_configs_generated PROPERTIES
	POSITION_INDEPENDENT_CODE ON)

target_link_libraries(nightshell_configs_generated PRIVATE
	Qt6::Core
	Qt6::Qml
	Qt6::Gui
	nightshell_utils
)

set(GENERATED_SOURCES
	generated/appearance.h
	generated/bar.h
	PARENT_SCOPE
)