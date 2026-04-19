cmake_minimum_required(VERSION 3.10)

project(VitaBrowserLite LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include("${VITASDK}/share/vita.toolchain.cmake" REQUIRED)
include("${VITASDK}/share/vita.cmake" REQUIRED)

add_executable(${PROJECT_NAME}
    src/main.cpp
)

target_link_libraries(${PROJECT_NAME}
    SceAppMgr_stub
    SceProcessmgr_stub
    SceDisplay_stub
    SceCtrl_stub
    SceNet_stub
    SceNetCtl_stub
    SceHttp_stub
    SceSysmem_stub
    SceGxm_stub
    SceCommonDialog_stub
    SceSysmodule_stub
    SceFont_stub          # ← Added
    SceSharedFb_stub      # ← Added
    vita2d
)

vita_create_self(eboot.bin ${PROJECT_NAME} UNSAFE)

vita_create_vpk(${PROJECT_NAME}.vpk "VBRS00001" eboot.bin
    VERSION "01.00"
    NAME "VitaBrowser Lite"
    FILE sce_sys sce_sys
)
