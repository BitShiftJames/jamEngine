
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
