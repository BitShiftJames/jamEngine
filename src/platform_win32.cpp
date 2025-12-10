
#include "platform_win32.h"
#include <cstdio>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>

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

void *gimme_function(void *dll_handle, char *function_name) {
  void *ptr = (void *)GetProcAddress((HMODULE)dll_handle, function_name);

  print_win32_error(GetLastError());

  return ptr;
}
