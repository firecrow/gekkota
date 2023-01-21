execute_process(COMMAND pkg-config wayland-protocols --variable=pkgdatadir OUTPUT_VARIABLE wayland_protocol_dir OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND pkg-config --variable=wayland_scanner wayland-scanner OUTPUT_VARIABLE wayland_scanner OUTPUT_STRIP_TRAILING_WHITESPACE)
set(xdg_shell_protocol ${wayland_protocol_dir}/stable/xdg-shell/xdg-shell.xml )

set(WAYLAND_TMP_DIR /tmp/gekkota_wayland_tmp)
message("Generating wayland .c and .h files into:" ${WAYLAND_TMP_DIR})
message(">>")
message(${wayland_scanner} client-header ${xdg_shell_protocol})
message("<<")

make_directory(${WAYLAND_TMP_DIR})
execute_process(COMMAND ${wayland_scanner} client-header ${xdg_shell_protocol} ${WAYLAND_TMP_DIR}/xdg-shell-client-protocol.h)
execute_process(COMMAND ${wayland_scanner} private-code ${xdg_shell_protocol} ${WAYLAND_TMP_DIR}/xdg-shell-protocol.c)

message("Declaring this modules output: FrontEndService")
add_library(frontend_service OBJECT FrontEndService.cpp ${WAYLAND_TMP_DIR}/xdg-shell-protocol.c)

target_link_libraries(frontend_service -lwayland-client)
message("including directories for wayland temporary headers: " ${WAYLAND_TMP_DIR})
target_include_directories(frontend_service PRIVATE ${WAYLAND_TMP_DIR})
target_include_directories(frontend_service PRIVATE ${CMAKE_SOURCE_DIR})