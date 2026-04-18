cmake_minimum_required(VERSION 3.0)

project(VitaBrowserLite)

set(CMAKE_C_STANDARD 11)

add_executable(${PROJECT_NAME}
    src/main.cpp
)

target_link_libraries(${PROJECT_NAME}
    SceAppUtil_stub
    SceNet_stub
    SceHttp_stub
    SceSsl_stub
)
