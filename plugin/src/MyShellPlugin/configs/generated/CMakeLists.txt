find_package(Qt6 REQUIRED COMPONENTS
    Core
    Qml
    Quick
)

qt6_add_library(myshell_configs_gen STATIC gen_includes.h)

target_sources(myshell_configs_gen PRIVATE
  barconfig.cpp
  mediaconfig.cpp
  notificationconfig.cpp
  appearanceconfig.cpp
  keymapconfig.cpp
  soundsconfig.cpp
  wallpaperconfig.cpp
  launcherconfig.cpp
  borderconfig.cpp
)

set_target_properties(myshell_configs_gen PROPERTIES
    POSITION_INDEPENDENT_CODE ON)

target_include_directories(myshell_configs_gen PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(myshell_configs_gen
    PRIVATE
        Qt::Core
        Qt::Qml
        Qt::Quick
        myshell_include
        myshell_utils)

set(GENERATED_SOURCES
    generated/barconfig.h generated/barconfig.cpp
	generated/mediaconfig.h generated/mediaconfig.cpp
	generated/notificationconfig.h generated/notificationconfig.cpp
	generated/appearanceconfig.h generated/appearanceconfig.cpp
	generated/keymapconfig.h generated/keymapconfig.cpp
	generated/soundsconfig.h generated/soundsconfig.cpp
	generated/wallpaperconfig.h generated/wallpaperconfig.cpp
	generated/launcherconfig.h generated/launcherconfig.cpp
	generated/borderconfig.h generated/borderconfig.cpp
    PARENT_SCOPE
)
