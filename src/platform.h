#ifndef PLATFORM_WIN32_H
#define PLATFORM_WIN32_H

#include "jamLibrary/RayAPI.h"

void recycle_delete(const char *path);
void *load_a_library(const char *path);
void *gimme_function(void *dll_handle, const char *function_name);
void unload_a_library(void *dll_handle);
void build_scenes(const char *path); 
void build_pathforScenes(); 
bool create_a_directory(const char *path);
bool directory_exist(const char *path);
FilePathList_ search_dynamic_libraries(RayAPI *engineCTX, const char *path);
bool IsValidLibraryExtension(const char *path, RayAPI *engineCTX);
void AppendLibraryExtension(char *buf, int *text_length, RayAPI *engineCTX);

#endif 
