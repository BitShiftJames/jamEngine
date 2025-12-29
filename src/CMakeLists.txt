cmake_minimum_required(VERSION 4.1.1)
project(Lumaria)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/jamScenes/compiled_scenes)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/jamScenes/compiled_scenes)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/jamScenes/compiled_scenes)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/jamScenes/compiled_scenes)

file(GLOB SCENE_FILES LIST_DIRECTORIES false CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/jamScenes/*.cpp")

add_library(jamLibrary STATIC IMPORTED)

if (WIN32)
  message("Building for windows")
  set_target_properties(jamLibrary PROPERTIES
      IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/jamLibrary.lib"
      INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/jamLibrary/"
      LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/jamScenes/compiled_scenes
      LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/jamScenes/compiled_scenes
      ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/jamScenes/compiled_scenes
      ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/jamScenes/compiled_scenes
  )
endif()

if (UNIX)
  message("Building for linux")
  set_target_properties(jamLibrary PROPERTIES
    IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/libjamLibrary.a"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/jamLibrary/"
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/jamScenes/compiled_scenes
    LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/jamScenes/compiled_scenes
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/jamScenes/compiled_scenes
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/jamScenes/compiled_scenes
  )
endif()

message("Source directory = ${CMAKE_SOURCE_DIR}")
message("Scene_files = ${SCENE_FILES}")

foreach(src ${SCENE_FILES})
  get_filename_component(scene_name ${src} NAME_WE)

  add_library(${scene_name} MODULE ${src})
  target_link_libraries(${scene_name} PRIVATE jamLibrary)

  set_target_properties(${scene_name} PROPERTIES
      PDB_NAME "${scene_name}_${BUILD_ID}"
      PREFIX ""
      LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/jamScenes/compiled_scenes
      LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/jamScenes/compiled_scenes
      ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/jamScenes/compiled_scenes
      ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/jamScenes/compiled_scenes
  )

endforeach()

