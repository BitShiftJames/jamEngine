cmake_minimum_required(VERSION 4.1.1)
project(Lumaria)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/jamScenes/compiled_scenes)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/jamScenes/compiled_scenes)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/jamScenes/compiled_scenes)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/jamScenes/compiled_scenes)


file(GLOB SCENE_FILES LIST_DIRECTORIES false CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/jamScenes/*.cpp")

message("External = ${CMAKE_SOURCE_DIR}/../external/raylib")
add_subdirectory(../external/raylib binary_dir)


message("Source directory = ${CMAKE_SOURCE_DIR}")
message("Scene_files = ${SCENE_FILES}")

foreach(src ${SCENE_FILES})
  get_filename_component(scene_name ${src} NAME_WE)

  add_library(${scene_name} MODULE ${src})

  target_link_libraries(${scene_name} raylib)

endforeach()
