cmake_minimum_required(VERSION 3.10)

project(VitaBrowserLite LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include Vita SDK toolchain
include("${VITASDK}/share/vita.toolchain.cmake" REQUIRED)

# Add your source files here
add_executable(${PROJECT_NAME}
    src/main.cpp
)

# Link required Vita libraries
target_link_libraries(${PROJECT_NAME}
    SceAppMgr_stub
    SceProcessmgr_stub
    SceDisplay_stub
    SceCtrl_stub
    SceNet_stub
    SceNetCtl_stub
    SceHttp_stub
    SceSysmem_stub
)

# Create the SELF (eboot.bin)
vita_create_self(eboot.bin ${PROJECT_NAME} UNSAFE)

# Create the VPK package
vita_create_vpk(${PROJECT_NAME}.vpk "VBRS00001" eboot.bin
    VERSION "01.00"
    NAME "VitaBrowser Lite"
    FILE sce_sys sce_sys
)

# Optional: allow "make install" to copy files locally for testing
install(TARGETS ${PROJECT_NAME} DESTINATION .)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/sce_sys DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
