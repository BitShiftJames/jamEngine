cmake_minimum_required(VERSION 4.1.1)
project(Lumaria)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/src)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(RAYLIB_STATIC ON CACHE BOOL "" FORCE)

file(GLOB jamLib_sources LIST_DIRECTORIES false ${CMAKE_SOURCE_DIR}/src/jamLibrary/*.cpp)

add_library(jamLibrary STATIC ${jamLib_sources})
set_target_properties(jamLibrary PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/src
  ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/src
)

add_subdirectory(external/raylib)

add_executable(${PROJECT_NAME} src/main.cpp src/platform_win32.cpp)
target_link_libraries(${PROJECT_NAME} raylib jamLibrary)
