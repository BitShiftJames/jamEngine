
#include "../platform.h"

#include <cstdio>
#include <cstdlib>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>

void unhandled_error() {
  printf("Unhandled error/exception.\n");
}

// TODO[String]: Functions to check if the string in null terminated.
void recycle_delete(const char *path) {
  SHFILEOPSTRUCTA fileoperation = {};
  fileoperation.wFunc = FO_DELETE;
  fileoperation.fFlags = FOF_NO_UI | FOF_ALLOWUNDO;
  fileoperation.pFrom = path;

  SHFileOperationA(&fileoperation);

}
void print_win32_error(DWORD error) {
  SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
  LPVOID msg;

  FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                 FORMAT_MESSAGE_FROM_SYSTEM |
                 FORMAT_MESSAGE_IGNORE_INSERTS, 
                 0, 
                 error, 
                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                 (LPSTR)&msg, 
                 0, 
                 0);

  printf("LoadLibrary failed with error %lu: %s\n", error, (char *)msg);
  LocalFree(msg);
}

void *load_a_library(const char *path) {
  void *ptr = LoadLibraryA(path);

  print_win32_error(GetLastError());

  return ptr;
}

void unload_a_library(void *dll_handle) {
  FreeLibrary((HMODULE)dll_handle);

  print_win32_error(GetLastError());

  return;
}

void *gimme_function(void *dll_handle, const char *function_name) {
  void *ptr = (void *)GetProcAddress((HMODULE)dll_handle, function_name);

  print_win32_error(GetLastError());

  return ptr;
}

void build_scenes(const char *path) {
  printf("path... %s\n", path);
  char buffer[2048];
  memset(buffer, 0, 2048);
  // Mhm hardcoded values.
  char programFiles[MAX_PATH];
  GetEnvironmentVariable("ProgramFiles", programFiles, MAX_PATH);

  snprintf(buffer, 2048, "call \"%s\\Microsoft Visual Studio\\2022\\Community\\Common7\\Tools\\VsDevCmd.bat\" && cd %s && cmake .. && msbuild Scene_compile.sln", programFiles, path);

  printf("Buffer command being run... %s\n", buffer);
  if (system(buffer) != 0) {
    unhandled_error();
  }
}

bool create_a_directory(const char *path) {
  bool result = false;
  BOOL return_value = CreateDirectoryA(path, 0);

  if (return_value == 0) {
    DWORD error = GetLastError();
    print_win32_error(error);
  } else {
    result = true;
  }

  return result;
}

bool directory_exist(const char *path) {
  bool result = false;
  DWORD file_attributes = GetFileAttributesA(path);
  if (file_attributes != INVALID_FILE_ATTRIBUTES) {
    if (file_attributes & FILE_ATTRIBUTE_DIRECTORY) {
      result = true;
    } else {
    }
  } else {
    DWORD error = GetLastError();
    print_win32_error(error);
  }
  return result;
}

FilePathList_ search_dynamic_libraries(RayAPI *engineCTX, const char *path) {
  FilePathList_ result = {};
  
  result = engineCTX->LoadDirectoryFiles(path, ".dll", false);

  return result;
}

bool IsValidLibraryExtension(const char *path, RayAPI *engineCTX) {
  bool result = engineCTX->IsFileExtension(path, ".dll");

  return result;
}

void AppendPathSeperator(char *buf, int *text_length, RayAPI *engineCTX) {
  engineCTX->TextAppend(buf, "\\", text_length);
}

void AppendLibraryExtension(char *buf, int *text_length, RayAPI *engineCTX) {
  engineCTX->TextAppend(buf, ".dll", text_length);
}

