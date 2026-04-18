cmake_minimum_required(VERSION 3.10)

project(VitaBrowserLite LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Vita-specific setup
include("${VITASDK}/share/vita.toolchain.cmake" REQUIRED)

add_executable(${PROJECT_NAME}
    src/main.cpp
)

target_link_libraries(${PROJECT_NAME}
    SceAppMgr_stub
    SceProcessmgr_stub
    # Add more libraries here as your browser grows (e.g. SceNet_stub, SceHttp_stub, vita2dlib, etc.)
)

# Create SELF (eboot.bin)
vita_create_self(eboot.bin ${PROJECT_NAME} UNSAFE)

# Create VPK
vita_create_vpk(${PROJECT_NAME}.vpk "VBRS00001" eboot.bin
    VERSION "01.00"
    NAME "VitaBrowser Lite"
    FILE sce_sys sce_sys
)

# Optional: install target for local development
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/sce_sys
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
