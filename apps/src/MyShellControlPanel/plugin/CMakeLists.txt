find_package(Qt6 REQUIRED COMPONENTS Core Quick)

cp_add_module(panel_plugin
  URI MyShellControlPanel.plugin
  SOURCES
    formcontroller.h formcontroller.cpp
    fieldcontroller.h fieldcontroller.cpp)

target_link_libraries(panel_plugin
  PRIVATE
    Qt::Core
    Qt::Quick)
