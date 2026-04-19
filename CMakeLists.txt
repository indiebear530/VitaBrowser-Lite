cmake_minimum_required(VERSION 3.10)
project(VitaBrowserLite LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

include("${VITASDK}/share/vita.cmake" REQUIRED)

add_executable(VitaBrowserLite
    src/main.cpp
)

target_link_libraries(VitaBrowserLite
    SceDisplay_stub
    SceCtrl_stub
    SceTouch_stub
    SceGxm_stub
    SceAppMgr_stub
    SceSysmodule_stub
    SceCommonDialog_stub
    SceFont_stub
    SceSharedFb_stub   # only if you really need it
)
