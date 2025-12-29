
#include "../platform.h"
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

void unhandled_error() {
  printf("Unhandled error/exception.\n");
}

void error_handling() {
  int errsv = errno;
  switch (errsv) {
    case (ENOENT): {
      // TODO: Trace log level.
      //printf("Your directory is in another castle.\n");
    } break;
    default: {
      strerror(errsv);
      unhandled_error();
    } break;
  }
}

void recycle_delete(const char *path) {
  if (unlink(path) != 0) {
    unhandled_error();
  }
}

void *load_a_library(const char *path) {
  void *result = dlopen(path, RTLD_LAZY);
  if (result == 0) {
    printf("Error loading a library\n");
    unhandled_error();
  }
  return result;
}

void *gimme_function(void *dll_handle, const char *function_name) {
  void *result = dlsym(dll_handle, function_name);
  if (result == 0) {
    printf("Error loading a function from a shared library\n");
    unhandled_error();
  }
  return result;
}

void unload_a_library(void *dll_handle) {
  if (dlclose(dll_handle) != 0) {
    unhandled_error();
  }
}

// Currently not implemented in the main engine.
void build_scenes(const char *path) {

  char buf[2048];

  // Mhm hardcoded values.
  snprintf(buf, 2048, "cd %s && cmake .. && make --makefile=Makefile", path);
  printf("Buffer command being run... %s\n", buf);
  if (system(buf) != 0) {
    unhandled_error();
  }
}

FilePathList_ search_dynamic_libraries(RayAPI *engineCTX, const char *path) {
  FilePathList_ result = {};
  
  result = engineCTX->LoadDirectoryFiles(path, ".so", false);

  return result;
}

bool IsValidLibraryExtension(const char *path, RayAPI *engineCTX) {
  bool result = engineCTX->IsFileExtension(path, ".so");
  return result;
}

void AppendLibraryExtension(char *buf, int *text_length, RayAPI *engineCTX) {
  engineCTX->TextAppend(buf, ".so", text_length);
}

void build_pathforScenes() {
  // TODO: Implement.
}

bool create_a_directory(const char *path) {
  bool result = false;
  if (mkdir(path, S_IRWXU | S_IRWXG) == -1) {
    error_handling();
  } else {
    result = true;
  }

  return result;
}

bool directory_exist(const char *path) {
  bool result = false;

  struct stat buf = {};

  if (stat(path, &buf) == -1) {
    error_handling();
  } else {
    result = S_ISDIR(buf.st_mode);
    //result ? printf("It is a directory\n") : printf("It isn't a directory :(\n");
  }
  
  return result;
}

