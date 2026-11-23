find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)

qt6_add_executable(nightshell main.cpp)

target_link_libraries(nightshell PRIVATE Qt6::Core Qt6::Gui Qt6::Qml)
