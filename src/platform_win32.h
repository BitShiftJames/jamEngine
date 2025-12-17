#ifndef PLATFORM_WIN32_H
#define PLATFORM_WIN32_H

void recycle_delete(const char *path);
void *load_a_library(const char *path);
void *gimme_function(void *dll_handle, const char *function_name);
void unload_a_library(void *dll_handle);

#endif 
