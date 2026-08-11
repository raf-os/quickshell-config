find_package(Qt6 REQUIRED COMPONENTS Core WaylandClient WaylandClientPrivate)
find_package(PkgConfig REQUIRED)
find_package(WaylandScanner REQUIRED)
pkg_check_modules(
  WAYLAND_CLIENT
  REQUIRED
  IMPORTED_TARGET
  wayland-client
  wayland-protocols>=1.41
)

if(NOT TARGET Wayland::Scanner)
  message(FATAL_ERROR "Wayland::Scanner target not found.")
endif()

if(NOT TARGET Qt6::qtwaylandscanner)
  message(FATAL_ERROR "qtwaylandscanner target not found.")
endif()

pkg_get_variable(WAYLAND_PROTOCOLS wayland-protocols pkgdatadir)

if(WAYLAND_PROTOCOLS)
  message(STATUS "Wayland protocols found at ${WAYLAND_PROTOCOLS}")
else()
  message(FATAL_ERROR "Unable to find wayland protocols.")
endif()

function(gen_wayland_protocol target name dir)
  if(NOT TARGET ${target})
    set(PROTO_BUILD_PATH ${CMAKE_BINARY_DIR}/wayland-protocols/${name})
    file(MAKE_DIRECTORY ${PROTO_BUILD_PATH})

    set(
      WS_CLIENT_HEADER
      "${PROTO_BUILD_PATH}/wayland-${name}-client-protocol.h"
    )
    set(WS_CLIENT_BODY "${PROTO_BUILD_PATH}/wayland-${name}.c")
    set(QWS_CLIENT_HEADER "${PROTO_BUILD_PATH}/qwayland-${name}.h")
    set(QWS_CLIENT_BODY "${PROTO_BUILD_PATH}/qwayland-${name}.cpp")
    set(PATH "${dir}/${name}.xml")

    add_custom_command(
      OUTPUT "${WS_CLIENT_HEADER}"
      COMMAND Wayland::Scanner client-header "${PATH}" "${WS_CLIENT_HEADER}"
      DEPENDS Wayland::Scanner "${PATH}"
    )

    add_custom_command(
      OUTPUT "${WS_CLIENT_BODY}"
      COMMAND Wayland::Scanner private-code "${PATH}" "${WS_CLIENT_BODY}"
      DEPENDS Wayland::Scanner "${PATH}"
    )

    add_custom_command(
      OUTPUT "${QWS_CLIENT_HEADER}"
      COMMAND
        Qt6::qtwaylandscanner client-header "${PATH}" > "${QWS_CLIENT_HEADER}"
      DEPENDS Qt6::qtwaylandscanner "${PATH}"
    )

    add_custom_command(
      OUTPUT "${QWS_CLIENT_BODY}"
      COMMAND Qt6::qtwaylandscanner client-code "${PATH}" > "${QWS_CLIENT_BODY}"
      DEPENDS Qt6::qtwaylandscanner "${PATH}"
    )

    add_library(${target} STATIC)

    target_sources(
      ${target}
      PUBLIC ${WS_CLIENT_HEADER} ${QWS_CLIENT_HEADER}
      PRIVATE ${WS_CLIENT_BODY} ${QWS_CLIENT_BODY}
    )

    target_include_directories(${target} PUBLIC ${PROTO_BUILD_PATH})

    target_link_libraries(
      ${target}
      PUBLIC
        Qt6::Core
        Qt6::WaylandClient
        Qt6::WaylandClientPrivate
        PkgConfig::WAYLAND_CLIENT
    )
  endif()
endfunction()
