#include "jamLibrary/RayAPI.h"
#include "jamLibrary/jamText.h"
#include "jamLibrary/jamTypes.h"
#include "jamLibrary/jamScene.h"

#include "platform.h"
#include "raylib.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

bool CheckLibraries(SceneList *scene_list, FilePathList_ *file_path_list, RayAPI *engineCTX) {
  if (!(scene_list && file_path_list && engineCTX)) {
    return false;
  }

  if (scene_list->scene_count != file_path_list->count) {
    return true;
  }

  for (u32 Index = 0; Index < file_path_list->count; Index++) {
    if (!TextEqual(file_path_list->paths[Index], scene_list->scene_path[Index])) {
      return true;
    }

    if (engineCTX->GetFileModTime(file_path_list->paths[Index]) != scene_list->scene_mod_time[Index]) {
      return true;
    }
  }

  return false;
}

struct src_file_list {
  u32 source_file_count;
  u64 *mod_time_list;
  char **src_path;
};

src_file_list Construct_source_list(FilePathList_ *List, memoryArena *src_file_memory, RayAPI *engineCTX) {

  memset(src_file_memory->memory, 0, src_file_memory->Used);
  src_file_memory->Used = 0;

  src_file_list src_files = {};
  src_files.source_file_count = List->count;
  src_files.src_path = PushArray(src_file_memory, src_files.source_file_count, char *);
  src_files.mod_time_list = PushArray(src_file_memory, src_files.source_file_count, u64);
  for (u32 Index = 0; Index < src_files.source_file_count; Index++) {
    u32 path_length = StringLength(List->paths[Index]) + 1;
    src_files.src_path[Index] = PushArray(src_file_memory, path_length, char);
    StringCopy(src_files.src_path[Index], List->paths[Index]);

    src_files.mod_time_list[Index] = engineCTX->GetFileModTime(src_files.src_path[Index]);
  }

  return src_files;
}

void CheckSrcFiles(char *src_path, char *build_path, src_file_list *source_files, memoryArena *src_file_memory, RayAPI *engineCTX) {
  FilePathList_ List = engineCTX->LoadDirectoryFiles(src_path, ".cpp", false);
  if (source_files->source_file_count != List.count) {

    // FIXME: Repeat code turn to function?
    *source_files = Construct_source_list(&List, src_file_memory, engineCTX);
    printf("Source File memory used %u vs Memory have %u\n", src_file_memory->Used, src_file_memory->Size);

    engineCTX->Build_scenes(build_path);
    return;
  }

  for (u32 Index = 0; Index < source_files->source_file_count; Index++) {
    if (!TextEqual(source_files->src_path[Index], List.paths[Index])) {

      // FIXME: Repeat code turn to function?
      *source_files = Construct_source_list(&List, src_file_memory, engineCTX);
      printf("Source File memory used %u vs Memory have %u\n", src_file_memory->Used, src_file_memory->Size);
    

      engineCTX->Build_scenes(build_path);
      return;
    }

    if (source_files->mod_time_list[Index] != engineCTX->GetFileModTime(List.paths[Index])) {

      // FIXME: Repeat code turn to function?
      *source_files = Construct_source_list(&List, src_file_memory, engineCTX);
      printf("Source File memory used %u vs Memory have %u\n", src_file_memory->Used, src_file_memory->Size);

      engineCTX->Build_scenes(build_path);
      return;
    }
  }

  engineCTX->UnloadDirectoryFiles(List);
};

void CheckForReload(char *scene_path, SceneList *sceneTable, memoryArena *dll_memory, RayAPI *engineCTX) {

  if (engineCTX->DoesDirectoryExist(scene_path)) {
    FilePathList_ List = engineCTX->SearchDynamicLibraries(engineCTX, scene_path);
    
    if (List.count == 0) {
      return;
    }

    // TOOD[Timing]: Once I have a good timer system I need to set a function on this check
    // to debounce because the build time is not instantaneous
    if (CheckLibraries(sceneTable, &List, engineCTX)) {
      { 
        ActiveScene *list = sceneTable->list;
        while (list) {
          if (list->scene) {
            if (list->scene->onExit) {
              list->scene->onExit(list->scene, engineCTX);
            }
          }
          list = list->next;
        }
      }

      Unload_scenes(sceneTable, engineCTX);

      ActiveScene *temp_list = sceneTable->list;       
      
      memset(dll_memory->memory, 0, dll_memory->Used);
      dll_memory->Used = 0;

      *sceneTable = Construct_scene_table(dll_memory, 128, scene_path, &List, engineCTX);

      sceneTable->list = temp_list;

      while (temp_list) {
        temp_list->scene = GetScene(sceneTable, temp_list->scene_name);
        if (temp_list->scene) {
          temp_list->scene->arena = temp_list->arena;
          if (temp_list->scene->onEnter) {
            temp_list->scene->onEnter(temp_list->scene, engineCTX);
          }
        }
        temp_list = temp_list->next;
      }
    }
    engineCTX->UnloadDirectoryFiles(List);

  }
}

void print_file_path_list(FilePathList_ *list) {
  if (!list->capacity || !list->count) {
    printf("There is nothing in this list");
  }

  for (u32 Index = 0; Index < list->count; Index++) {
    printf("Index: %u, Path: %s\n", Index, list->paths[Index]);
  }
}

int main() {
  RayAPI engineCTX = {};

  engineCTX.UsersafeDelete = (tUsersafeDeleteFile)recycle_delete;
  engineCTX.LoadDLLFromPath = (tLoadDLLFromPath)load_a_library;
  engineCTX.UnloadDLLFromPath = (tUnloadDLLFromPath)unload_a_library;
  engineCTX.LoadFunctionFromDLL = (tLoadFunctionFromDLL)gimme_function;
  engineCTX.Build_scenes = (tBuild_scenes)build_scenes;
  engineCTX.DoesDirectoryExist = (tDoesDirectoryExist)directory_exist;
  engineCTX.CreateDirectory = (tCreateDirectory)create_a_directory;
  engineCTX.SearchDynamicLibraries = (tSearchDynamicLibraries)search_dynamic_libraries;
  engineCTX.IsValidLibraryExtension = (tIsValidLibraryExtension)IsValidLibraryExtension;
  engineCTX.AppendLibraryExtension = (tAppendLibraryExtension)AppendLibraryExtension;
  engineCTX.AppendPathSeperator = (tAppendPathSeperator)AppendPathSeperator;

  engineCTX.ClearBackground = (tClearBackground)ClearBackground;

  engineCTX.BeginDrawing = (tBeginDrawing)BeginDrawing;
  engineCTX.EndDrawing = (tEndDrawing)EndDrawing;

  engineCTX.BeginMode2D = (tBeginMode2D)BeginMode2D;
  engineCTX.EndMode2D = (tEndMode2D)EndMode2D;

  engineCTX.BeginMode3D = (tBeginMode3D)BeginMode3D;
  engineCTX.EndMode3D = (tEndMode3D)EndMode3D;

  engineCTX.DrawLine = (tDrawLine)DrawLine;
  engineCTX.DrawCircle = (tDrawCircle)DrawCircleSector;
  engineCTX.DrawEllipse = (tDrawEllipse)DrawEllipseV;
  engineCTX.DrawRectangle = (tDrawRectangle)DrawRectangleV;
  engineCTX.DrawTriangle = (tDrawTriangle)DrawTriangle;
  engineCTX.DrawPoly = (tDrawPoly)DrawPoly;
  engineCTX.DrawText = (tDrawText)DrawTextEx;

  engineCTX.GetFontDefault = (tGetFontDefault)GetFontDefault;
  engineCTX.LoadFont = (tLoadFont)LoadFont;
  engineCTX.UnloadFont = (tUnloadFont)UnloadFont;
  engineCTX.MeasureText = (tMeasureText)MeasureTextEx;
  
  engineCTX.ScreenSize = {(f32)GetScreenWidth(), (f32)GetScreenHeight()};
  
  engineCTX.FileExists = (tFileExist)FileExists;
  engineCTX.FileRemove = (tFileRemove)FileRemove;
  engineCTX.FileCopy = (tFileCopy)FileCopy;
  engineCTX.GetWorkingDirectory = (tGetWorkingDirectory)GetWorkingDirectory;
  engineCTX.GetDirectoryPath = (tGetDirectoryPath)GetDirectoryPath;
  engineCTX.LoadDirectoryFiles = (tLoadDirectoryFiles)LoadDirectoryFilesEx;
  engineCTX.UnloadDirectoryFiles = (tUnloadDirectoryFiles)UnloadDirectoryFiles;
  engineCTX.IsFileExtension = (tIsFileExtension)IsFileExtension;
  engineCTX.GetFileNameWithoutExt = (tGetFileNameWithoutExt)GetFileNameWithoutExt;
  engineCTX.GetFileModTime = (tGetFileModTime)GetFileModTime;
  
  engineCTX.InitWindow = (tInitWindow)InitWindow;
  engineCTX.CloseWindow = (tCloseWindow)CloseWindow;
  engineCTX.WindowShouldClose = (tWindowShouldClose)WindowShouldClose;
  engineCTX.GetScreenWidth = (tGetScreenWidth)GetScreenWidth;
  engineCTX.GetScreenHeight = (tGetScreenHeight)GetScreenHeight;
  engineCTX.IsWindowResized = (tIsWindowResized)IsWindowResized;

  engineCTX.MemAlloc = (tMemAlloc)MemAlloc;
  engineCTX.MemRealloc = (tMemRealloc)MemRealloc;
  engineCTX.MemFree = (tMemFree)MemFree;

  engineCTX.SetConfigFlags = (tSetConfigFlags)SetConfigFlags;
  engineCTX.SetTargetFPS = (tSetTargetFPS)SetTargetFPS;
  engineCTX.SetTraceLogLevel = (tSetTraceLogLevel)SetTraceLogLevel;

  engineCTX.TextAppend = (tTextAppend)TextAppend;
  engineCTX.TextFormat = (tTextFormat)TextFormat;

  engineCTX.DrawLine3D = (tDrawLine3D)DrawLine3D;
  engineCTX.DrawPoint3D = (tDrawPoint3D)DrawPoint3D;
  engineCTX.DrawCircle3D = (tDrawCircle3D)DrawCircle3D;
  engineCTX.DrawTriangle3D = (tDrawTriangle3D)DrawTriangle3D;
  engineCTX.DrawCube = (tDrawCube)DrawCubeV;
  engineCTX.DrawSphere = (tDrawSphere)DrawSphereEx;
  engineCTX.DrawCylinder = (tDrawCylinder)DrawCylinderEx;
  engineCTX.DrawCapsule = (tDrawCapsule)DrawCapsule;
  engineCTX.DrawPlane = (tDrawPlane)DrawPlane;
  engineCTX.DrawRay = (tDrawRay)DrawRay;
  engineCTX.DrawGrid = (tDrawGrid)DrawGrid;
  engineCTX.DrawWireframeCube = (tDrawWireframeCube)DrawCubeWiresV;
  engineCTX.DrawWireframeSphere = (tDrawWireframeSphere)DrawSphereWires;
  engineCTX.DrawWireframeCylinder = (tDrawWireframeCylinder)DrawCylinderWiresEx;
  engineCTX.DrawWireframeCapsule = (tDrawWireframeCapsule)DrawCapsuleWires;

  engineCTX.GenMeshPoly = (tGenMeshPoly)GenMeshPoly;
  engineCTX.GenMeshPlane = (tGenMeshPlane)GenMeshPlane;
  engineCTX.GenMeshCube = (tGenMeshCube)GenMeshCube;
  engineCTX.GenMeshSphere = (tGenMeshSphere)GenMeshSphere;
  engineCTX.GenMeshCylinder = (tGenMeshCylinder)GenMeshCylinder;
  engineCTX.GenMeshCone = (tGenMeshCone)GenMeshCone;
  engineCTX.GenMeshTorus = (tGenMeshTorus)GenMeshTorus;
  engineCTX.GenMeshKnot = (tGenMeshKnot)GenMeshKnot;
  engineCTX.GenMeshHeightMap = (tGenMeshHeightMap)GenMeshHeightmap;
  engineCTX.GenMeshCubicmap = (tGenMeshCubicmap)GenMeshCubicmap;

  engineCTX.LoadModel = (tLoadModel)LoadModel;
  engineCTX.LoadModelFromMesh = (tLoadModelFromMesh)LoadModelFromMesh;
  engineCTX.IsModelValid = (tIsModelValid)IsModelValid;
  engineCTX.UnloadModel = (tUnloadModel)UnloadModel;
  engineCTX.GetModelBoundingBox = (tGetModelBoundingBox)GetModelBoundingBox;

  engineCTX.DrawModel = (tDrawModel)DrawModel;
  engineCTX.DrawWireframe = (tDrawWireframe)DrawModelWiresEx;
  engineCTX.DrawModelPoints = (tDrawModelPoints)DrawModelPointsEx;
  engineCTX.DrawBillboard = (tDrawBillboard)DrawBillboardPro;

  engineCTX.UploadMesh = (tUploadMesh)UploadMesh;
  engineCTX.DrawMesh = (tDrawMesh)DrawMesh;
  engineCTX.DrawMeshInstanced = (tDrawMeshInstanced)DrawMeshInstanced;
  engineCTX.GetMeshBoundingBox = (tGetMeshBoundingBox)GetMeshBoundingBox;
  engineCTX.GenMeshTangents = (tGenMeshTangents)GenMeshTangents;
  engineCTX.ExportMesh = (tExportMesh)ExportMesh;

  engineCTX.LoadMaterials = (tLoadMaterials)LoadMaterials;
  engineCTX.LoadMaterialsDefault = (tLoadMaterialsDefault)LoadMaterialDefault;
  engineCTX.UnloadMaterial = (tUnloadMaterial)UnloadMaterial;
  engineCTX.IsMaterialValid = (tIsMaterialValid)IsMaterialValid;
  engineCTX.SetMaterialTexture = (tSetMaterialTexture)SetMaterialTexture;
  engineCTX.SetModelMeshMaterial = (tSetModelMeshMaterial)SetModelMeshMaterial;

  engineCTX.IsMouseButtonPressed = (tIsMouseButtonPressed)IsMouseButtonPressed;
  engineCTX.IsMouseButtonDown = (tIsMouseButtonDown)IsMouseButtonDown;
  engineCTX.IsMouseButtonReleased = (tIsMouseButtonReleased)IsMouseButtonReleased;
  engineCTX.IsMouseButtonUp = (tIsMouseButtonUp)IsMouseButtonUp;
  engineCTX.GetMouseDelta = (tGetMouseDelta)GetMouseDelta;
  engineCTX.GetMousePosition = (tGetMousePosition)GetMousePosition;
  engineCTX.GetWheelMove = (tGetWheelMove)GetMouseWheelMoveV;

  engineCTX.IsKeyPressed = (tIsKeyPressed)IsKeyPressed;
  engineCTX.IsKeyPressedRepeat = (tIsKeyPressedRepeat)IsKeyPressedRepeat;
  engineCTX.IsKeyDown = (tIsKeyDown)IsKeyDown;
  engineCTX.IsKeyReleased = (tIsKeyReleased)IsKeyReleased;
  engineCTX.IsKeyUp = (tIsKeyUp)IsKeyUp;
  engineCTX.GetKeyPressed = (tGetKeyPressed)GetKeyPressed;
  engineCTX.GetCharPressed = (tGetCharPressed)GetCharPressed;
  engineCTX.GetKeyName = (tGetKeyName)GetKeyName;
  engineCTX.SetExitKey = (tSetExitKey)SetExitKey;

  engineCTX.UpdateCamera = (tUpdateCamera)UpdateCamera;

  engineCTX.ShowCursor = (tShowCursor)ShowCursor;
  engineCTX.HideCursor = (tHideCursor)HideCursor;
  engineCTX.IsCursorHidden = (tIsCursorHidden)IsCursorHidden;
  engineCTX.EnableCursor = (tEnableCursor)EnableCursor;
  engineCTX.DisableCursor = (tDisableCursor)DisableCursor;
  engineCTX.IsCursorOnScreen = (tIsCursorOnScreen)IsCursorOnScreen;
  
  engineCTX.GetFrameTime = (tGetFrameTime)GetFrameTime;

  // TODO[Refactor]: Have scenes suballocate out of a bigger memory block
  memoryArena scene_memory = {};
  scene_memory.Size = Megabytes(200);
  scene_memory.memory = engineCTX.MemAlloc(scene_memory.Size);

  memoryArena dll_memory = {};
  dll_memory.Size = Megabytes(1);
  dll_memory.memory = engineCTX.MemAlloc(dll_memory.Size);

  memoryArena active_scene_list_memory = {};
  active_scene_list_memory.Size = Megabytes(1);
  active_scene_list_memory.memory = engineCTX.MemAlloc(active_scene_list_memory.Size);

  memoryArena src_file_memory = {};
  src_file_memory.Size = Megabytes(1);
  src_file_memory.memory = engineCTX.MemAlloc(src_file_memory.Size);

  memoryArena paths_memory = {};
  paths_memory.Size = Kilobytes(3);
  paths_memory.memory = engineCTX.MemAlloc(paths_memory.Size);

  u32 flags = FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT;// FLAG_WINDOW_TOPMOST | FLAG_WINDOW_UNDECORATED;
  engineCTX.SetConfigFlags(flags);
  // passing 0 makes raylib window the size of the screen.
  engineCTX.InitWindow(0, 0, "Restarting from scratch");
  engineCTX.SetTraceLogLevel(LOG_ALL);
  engineCTX.SetTargetFPS(30);
  
  char *scene_path;
  char *src_path;
  char *scene_build_dir;
  SceneList sceneTable = {};
  src_file_list srcList = {};
  {
    {
      char *working_directory = (char *)engineCTX.GetWorkingDirectory();
      int text_length = StringLength(working_directory);
      engineCTX.AppendPathSeperator(working_directory, &text_length, &engineCTX);
      engineCTX.TextAppend(working_directory, "build", &text_length);

      scene_build_dir = PushArray(&paths_memory, StringLength(working_directory) + 1, char);
      StringCopy(scene_build_dir, working_directory);
      
      printf("Scene build directory %s\n", scene_build_dir);

      if (engineCTX.DoesDirectoryExist(scene_build_dir)) {
        build_scenes(scene_build_dir);
      } else {
        engineCTX.CreateDirectory(scene_build_dir);
        build_scenes(scene_build_dir);
      }
    }

    char *working_directory = (char *)engineCTX.GetWorkingDirectory();
    
    int text_length = StringLength(working_directory);
    engineCTX.AppendPathSeperator(working_directory, &text_length, &engineCTX);
    engineCTX.TextAppend(working_directory, "jamScenes", &text_length);


    src_path = PushArray(&paths_memory, StringLength(working_directory) + 1, char);
    StringCopy(src_path, working_directory);

    printf("Showing source path: %s\n", src_path);

    text_length = StringLength(working_directory);
    engineCTX.AppendPathSeperator(working_directory, &text_length, &engineCTX);
    engineCTX.TextAppend(working_directory, "compiled_scenes", &text_length);


    scene_path = PushArray(&paths_memory, StringLength(working_directory) + 1, char);
    StringCopy(scene_path, working_directory);

    printf("Showing scene path: %s\n", scene_path);

    {
      FilePathList_ List = engineCTX.LoadDirectoryFiles(src_path, ".cpp", false);
      srcList = Construct_source_list(&List, &src_file_memory, &engineCTX);
      engineCTX.UnloadDirectoryFiles(List);
    }

    {
      if (engineCTX.DoesDirectoryExist(scene_path)) {
        FilePathList_ List = engineCTX.SearchDynamicLibraries(&engineCTX, scene_path);

        print_file_path_list(&List);
        sceneTable = Construct_scene_table(&dll_memory, 128, scene_path, &List, &engineCTX);
        engineCTX.UnloadDirectoryFiles(List);
      } else {
        engineCTX.CreateDirectory(scene_path);
      }
    }

  }


  AddScene(&sceneTable, (char *)"camera_rotation", &active_scene_list_memory, &scene_memory, Megabytes(4), &engineCTX);

  engineCTX.ScreenSize = {(f32)engineCTX.GetScreenWidth(), (f32)engineCTX.GetScreenHeight()};
  engineCTX.HalfScreenSize = engineCTX.ScreenSize / 2;
  
  while (!engineCTX.WindowShouldClose()) {

    CheckSrcFiles(src_path, scene_build_dir, &srcList, &src_file_memory, &engineCTX);
    CheckForReload(scene_path, &sceneTable, &dll_memory, &engineCTX);
  
    if (engineCTX.IsWindowResized()) {
      engineCTX.ScreenSize = {(f32)engineCTX.GetScreenHeight(), (f32)engineCTX.GetScreenHeight()};
      engineCTX.HalfScreenSize = engineCTX.ScreenSize / 2;
    }

    {
      Vector2 mousepos = GetMousePosition();
      engineCTX.MousePosition = {mousepos.x, mousepos.y};
    }

    {
      ActiveScene *currNode = sceneTable.list;

      while (currNode != 0) {
        if (currNode->scene) {
          currNode->scene->update(currNode->scene, &engineCTX);
        }
        currNode = currNode->next;
      }

    }

    BeginDrawing();
    {
      ActiveScene *currNode = sceneTable.list;

      while (currNode != 0) {
        if (currNode->scene) {
          currNode->scene->render(currNode->scene, &engineCTX);
        }
        currNode = currNode->next;
      }
    }
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
