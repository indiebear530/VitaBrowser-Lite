cmake_minimum_required(VERSION 3.5)

project(vita_browser)

include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,-q -Wall -O3")

add_executable(vita_browser
    src/main.c
)

target_link_libraries(vita_browser
    vita2d
    SceCtrl_stub
    SceDisplay_stub
    SceGxm_stub
    SceSysmodule_stub
    SceTouch_stub
    SceHttp_stub
    SceNet_stub
    SceNetCtl_stub
    SceKernel_stub
)

vita_create_self(eboot.bin vita_browser UNSAFE)

vita_create_vpk(vita_browser.vpk "VITABRWS" eboot.bin
    VERSION "01.00"
    NAME "Vita Browser"
)
