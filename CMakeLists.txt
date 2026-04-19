cmake_minimum_required(VERSION 3.10)

project(VitaBrowserLite LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Toolchain is expected to be provided via:
#   -DCMAKE_TOOLCHAIN_FILE=$VITASDK/share/vita.toolchain.cmake
# (or equivalent in your preset/environment).
#
# Resolve VITASDK for vita.cmake in a robust way across local and CI setups.
if(NOT DEFINED VITASDK AND DEFINED ENV{VITASDK})
    set(VITASDK "$ENV{VITASDK}")
endif()

if(NOT DEFINED VITASDK)
    message(FATAL_ERROR "VITASDK is not set. Export VITASDK and pass -DCMAKE_TOOLCHAIN_FILE=$VITASDK/share/vita.toolchain.cmake")
endif()

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
    vita2d
)

vita_create_self(eboot.bin ${PROJECT_NAME} UNSAFE)

vita_create_vpk(${PROJECT_NAME}.vpk "VBRS00001" eboot.bin
    VERSION "01.00"
    FILE sce_sys sce_sys
)

install(TARGETS ${PROJECT_NAME} DESTINATION .)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/sce_sys DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
