cmake_minimum_required(VERSION 4.1.1)
project(Lumaria)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/src)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_executable(${PROJECT_NAME} src/main.cpp src/safe_delete.cpp)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
file(GLOB SCENE_FILES LIST_DIRECTORIES false )

add_subdirectory(external/raylib)
target_link_libraries(${PROJECT_NAME} raylib)
