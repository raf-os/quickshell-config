find_package(Qt6 REQUIRED COMPONENTS
    Core
    Qml
    Quick
)

add_library(myshell_configs_gen
    STATIC
        barconfig.h barconfig.cpp
		mediaconfig.h mediaconfig.cpp
		appearanceconfig.h appearanceconfig.cpp
		keymapconfig.h keymapconfig.cpp
		launcherconfig.h launcherconfig.cpp
		borderconfig.h borderconfig.cpp
        gen_includes.h)

set_target_properties(myshell_configs_gen PROPERTIES
    POSITION_INDEPENDENT_CODE ON)

target_include_directories(myshell_configs_gen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(myshell_configs_gen
    PRIVATE
        Qt::Core
        Qt::Qml
        Qt::Quick
        myshell_include)

set(GENERATED_SOURCES
    generated/barconfig.h generated/barconfig.cpp
	generated/mediaconfig.h generated/mediaconfig.cpp
	generated/appearanceconfig.h generated/appearanceconfig.cpp
	generated/keymapconfig.h generated/keymapconfig.cpp
	generated/launcherconfig.h generated/launcherconfig.cpp
	generated/borderconfig.h generated/borderconfig.cpp
    PARENT_SCOPE
)