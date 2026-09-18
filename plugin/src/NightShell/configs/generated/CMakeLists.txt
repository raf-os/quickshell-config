find_package(Qt6 REQUIRED COMPONENTS Core Qml)

qt6_add_library(nightshell_configs_generated STATIC)

target_sources(nightshell_configs_generated PRIVATE
	appearance.cpp
	bar.cpp
)

target_include_directories(nightshell_configs_generated PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(nightshell_configs_generated PRIVATE
	Qt6::Core
	Qt6::Qml
	nightshell_utils
)

set(GENERATED_SOURCES
	generated/appearance.cpp
	generated/bar.cpp
	PARENT_SCOPE
)