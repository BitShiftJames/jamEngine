cmake_minimum_required(VERSION 4.1.1)
project(Lumaria)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/src)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(RAYLIB_STATIC ON CACHE BOOL "" FORCE)

add_subdirectory(external/raylib)

add_executable(${PROJECT_NAME} 
  src/main.cpp 
  src/jamLibrary/jamText.cpp 
  src/jamLibrary/jamScene.cpp 
  src/jamLibrary/jamUI.cpp 
  src/platform_win32.cpp)

target_link_libraries(${PROJECT_NAME} raylib)
