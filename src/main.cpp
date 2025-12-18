#include "jamLibrary/RayAPI.h"
#include "jamLibrary/jamText.h"
#include "jamLibrary/jamTypes.h"
#include "jamLibrary/jamScene.h"

#include "platform_win32.h"
#include "raylib.h"

#include <cstdlib>
#include <cstring>

#if 0
#define CURSOR_SIDE 32
#define CURSOR_HALF_SIDE 16

struct CursorObject {
  Rectangle layer1;
  Rectangle layer2;
  Rectangle layer3;

  // TODO: Settings.
  Color layer1Tint = {0, 0, 0, 255};
  Color layer2Tint = {255, 255, 255, 255};
  Color layer3Tint = {100, 0, 0, 255};

  Texture2D texture;

  Cursor_inventory_information Inventory;
};
#endif

bool CheckLibraries(SceneList *scene_list, FilePathList_ *file_path_list, RayAPI *engineCTX) {
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

    if (!system(build_path)) {
      // TODO[Error handling];
    } 
    return;
  }

  for (u32 Index = 0; Index < source_files->source_file_count; Index++) {
    if (!TextEqual(source_files->src_path[Index], List.paths[Index])) {

      // FIXME: Repeat code turn to function?
      *source_files = Construct_source_list(&List, src_file_memory, engineCTX);

      if (!system(build_path)) {
        // TODO[Error handling];
      } 
      return;
    }

    if (source_files->mod_time_list[Index] != engineCTX->GetFileModTime(List.paths[Index])) {

      // FIXME: Repeat code turn to function?
      *source_files = Construct_source_list(&List, src_file_memory, engineCTX);

      if (!system(build_path)) {
        // TODO[Error handling];
      } 
      return;
    }
  }

  engineCTX->UnloadDirectoryFiles(List);
};

void CheckForReload(char *scene_path, SceneList *sceneTable, memoryArena *dll_memory, RayAPI *engineCTX) {

  FilePathList_ List = engineCTX->LoadDirectoryFiles(scene_path, ".dll", false);

  if (List.count == 0) {
    return;
  }
  
  // TOOD[Timing]: Once I have a good timer system I need to set a function on this check
  // to debounce because the build time is not instantaneous
  if (CheckLibraries(sceneTable, &List, engineCTX)) {

    {
      ActiveScene *list = sceneTable->list;
      while (list) {
        if (list->scene->onExit) {
          list->scene->onExit(list->scene);
        }
        list = list->next;
      }
    }

    Unload_scenes(sceneTable, engineCTX);

    ActiveScene *temp_list = sceneTable->list;       

    memset(dll_memory->memory, 0, dll_memory->Used);
    *sceneTable = Construct_scene_table(dll_memory, 128, scene_path, &List, engineCTX);

    sceneTable->list = temp_list;

    while (temp_list) {
      temp_list->scene = GetScene(sceneTable, temp_list->scene_name);
      temp_list->scene->arena = temp_list->arena;
      if (temp_list->scene->onEnter) {
        temp_list->scene->onEnter(temp_list->scene, engineCTX);
      }
      temp_list = temp_list->next;
    }

  }
  engineCTX->UnloadDirectoryFiles(List);

}

int main() {
  RayAPI engineCTX = {};
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
  
  engineCTX.UsersafeDelete = (tUsersafeDeleteFile)recycle_delete;
  engineCTX.LoadDLLFromPath = (tLoadDLLFromPath)load_a_library;
  engineCTX.UnloadDLLFromPath = (tUnloadDLLFromPath)unload_a_library;
  engineCTX.LoadFunctionFromDLL = (tLoadFunctionFromDLL)gimme_function;
  
  engineCTX.InitWindow = (tInitWindow)InitWindow;
  engineCTX.CloseWindow = (tCloseWindow)CloseWindow;
  engineCTX.WindowShouldClose = (tWindowShouldClose)WindowShouldClose;
  engineCTX.GetScreenWidth = (tGetScreenWidth)GetScreenWidth;
  engineCTX.GetScreenHeight = (tGetScreenHeight)GetScreenHeight;

  engineCTX.MemAlloc = (tMemAlloc)MemAlloc;
  engineCTX.MemRealloc = (tMemRealloc)MemRealloc;
  engineCTX.MemFree = (tMemFree)MemFree;

  engineCTX.SetConfigFlags = (tSetConfigFlags)SetConfigFlags;
  engineCTX.SetTargetFPS = (tSetTargetFPS)SetTargetFPS;
  engineCTX.SetTraceLogLevel = (tSetTraceLogLevel)SetTraceLogLevel;

  engineCTX.TextAppend = (tTextAppend)TextAppend;

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
  

  #if 0
  CursorObject global_cursor = {};
  global_cursor.texture = LoadTexture("../assets/Cursor.png");

  global_cursor.layer1 = {0, 0, (f32)global_cursor.texture.width, (f32)((u32)(global_cursor.texture.height / 3))};
  global_cursor.layer2 = {0, CURSOR_SIDE, (f32)global_cursor.texture.width, (f32)((u32)(global_cursor.texture.height / 3))};
  global_cursor.layer3 = {0, CURSOR_SIDE * 2, (f32)global_cursor.texture.width, (f32)((u32)(global_cursor.texture.height / 3))};

  Cursor_inventory_information CursorInventory = {};:
  CursorInventory.dest_rect = JamRectMinDim(v2{0, 0}, v2{16, 16});
  global_cursor.Inventory = CursorInventory;
  #endif


  char *scene_path;
  char *src_path;
  char *build_file_path;
  SceneList sceneTable = {};
  src_file_list srcList = {};
  {
    char *working_directory = (char *)engineCTX.GetWorkingDirectory();
    {
      FilePathList_ List = engineCTX.LoadDirectoryFiles(working_directory, ".bat", true);
      if (List.count == 1) {
        int Text_Length = 0;
        build_file_path = PushArray(&paths_memory, 2048, char);
        engineCTX.TextAppend(build_file_path, "cd ", &Text_Length);

        engineCTX.TextAppend(build_file_path, engineCTX.GetDirectoryPath(List.paths[0]), &Text_Length);

        engineCTX.TextAppend(build_file_path, " && ", &Text_Length);

        engineCTX.TextAppend(build_file_path, List.paths[0], &Text_Length);
      } else {
        // Error handling.
      }
      engineCTX.UnloadDirectoryFiles(List);

    }

    int text_length = StringLength(working_directory);
    engineCTX.TextAppend(working_directory, "\\jamScenes", &text_length);

    src_path = PushArray(&paths_memory, StringLength(working_directory) + 1, char);
    StringCopy(src_path, working_directory);

    text_length = StringLength(working_directory);
    engineCTX.TextAppend(working_directory, "\\compiled_scenes", &text_length);

    scene_path = PushArray(&paths_memory, StringLength(working_directory) + 1, char);
    StringCopy(scene_path, working_directory);

    {
      FilePathList_ List = engineCTX.LoadDirectoryFiles(src_path, ".cpp", false);
      srcList = Construct_source_list(&List, &src_file_memory, &engineCTX);
      engineCTX.UnloadDirectoryFiles(List);
    }

    // TODO [Polish]:
    // This could be built to auto detect if the mod time changed from last known time.
    // Which would require saving the mod time to disk.
    if (build_file_path) {
      if (!system(build_file_path)) {
        // Error Handling...
      }
    }

    {
      FilePathList_ List = engineCTX.LoadDirectoryFiles(scene_path, ".dll", false);
      sceneTable = Construct_scene_table(&dll_memory, 128, scene_path, &List, &engineCTX);
      engineCTX.UnloadDirectoryFiles(List);
    }

  }


  AddScene(&sceneTable, (char *)"uiScene", &active_scene_list_memory, &scene_memory, Megabytes(4), &engineCTX);
  
  while (!engineCTX.WindowShouldClose()) {

    CheckSrcFiles(src_path, build_file_path, &srcList, &src_file_memory, &engineCTX);
    CheckForReload(scene_path, &sceneTable, &dll_memory, &engineCTX);

    {
    Vector2 mousepos = GetMousePosition();
    engineCTX.MousePosition = {mousepos.x, mousepos.y};
    }

    {
      ActiveScene *currNode = sceneTable.list;

      while (currNode != 0) {
        if (currNode->scene) {
          currNode->scene->update(currNode->scene);
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
    //

  }

  CloseWindow();
  return 0;
}
