cmake_minimum_required(VERSION 3.10)
project(VitaBrowserLite LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# Only include vita.cmake (toolchain is passed via command line / CI)
include("${VITASDK}/share/vita.cmake" REQUIRED)

add_executable(${PROJECT_NAME}
    src/main.cpp
)

target_link_libraries(${PROJECT_NAME}
    vita2d
    SceDisplay_stub
    SceCtrl_stub
    SceGxm_stub
    SceSysmodule_stub
    SceFont_stub
    SceNet_stub
    SceNetCtl_stub
    SceHttp_stub
    ScePgf_stub          # ← Fixed: required for vita2d_pgf functions
)

vita_create_self(eboot.bin ${PROJECT_NAME} UNSAFE)
vita_create_vpk(${PROJECT_NAME}.vpk "VBRS00001" eboot.bin
    VERSION "01.00"
    NAME "VitaBrowser Lite"
    FILE sce_sys sce_sys
)
