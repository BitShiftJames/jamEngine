#include "jamLibrary/RayAPI.h"
#include "jamLibrary/jamText.h"
#include "jamLibrary/jamTypes.h"
#include "jamLibrary/jamScene.h"

#include "platform_win32.h"
#include "raylib.h"
#include <cstdio>
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

bool CheckFilePath(SceneList *scene_list, FilePathList *file_path_list) {
   

  if (scene_list->scene_count != file_path_list->count) {
    return true;
  }

  for (u32 Index = 0; Index < file_path_list->count; Index++) {
    if (!TextEqual(file_path_list->paths[Index], scene_list->scene_path[Index])) {
      return true;
    }

    if (GetFileModTime(file_path_list->paths[Index]) != scene_list->scene_mod_time[Index]) {
      return true;
    }
  }

  return false;
}
void CheckForReload(char *scene_path, SceneList *sceneTable, memoryArena *sceneMemory) {

  FilePathList List = LoadDirectoryFilesEx(scene_path, ".dll", false);

  if (List.count == 0) {
    return;
  }

  // TOOD[Timing]: Once I have a good timer system I need to set a function on this check
  // to debounce because the build time is not instantaneous
  if (CheckFilePath(sceneTable, &List)) {

    Unload_scenes(sceneTable);

    ActiveScene *list = sceneTable->list;       

    memset(sceneMemory->memory, 0, sceneMemory->Used);
    *sceneTable = Construct_scene_table(sceneMemory, 128, scene_path, &List);

    sceneTable->list = list;

    while (list) {
      list->scene = GetScene(sceneTable, list->scene_name);
      list = list->next;
    }

  }
  UnloadDirectoryFiles(List);
}
int main() {
  u32 flags = FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT;// FLAG_WINDOW_TOPMOST | FLAG_WINDOW_UNDECORATED;
  SetConfigFlags(flags);
  // passing 0 makes raylib window the size of the screen.
  InitWindow(0, 0, "Restarting from scratch");
  SetTraceLogLevel(LOG_ALL);
  SetTargetFPS(30);
  
  s32 ScreenWidth = GetScreenWidth();
  s32 ScreenHeight = GetScreenHeight();

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
  
  memoryArena scene_memory = {};
  scene_memory.Size = Megabytes(1);
  scene_memory.memory = MemAlloc(scene_memory.Size);
  
  memoryArena active_scene_memory = {};
  active_scene_memory.Size = Megabytes(1);
  active_scene_memory.memory = MemAlloc(scene_memory.Size);

  memoryArena scene_path_memory = {};
  scene_path_memory.Size = Kilobytes(3);
  scene_path_memory.memory = MemAlloc(scene_path_memory.Size);

  char *scene_path;
  {
    char *working_directory = (char *)GetWorkingDirectory();
    int text_length = StringLength(working_directory);
    TextAppend(working_directory, "\\jamScenes\\compiled_scenes", &text_length);

    scene_path = PushArray(&scene_path_memory, StringLength(working_directory), char);
    TextCopy(scene_path, working_directory);
  }

  RayAPI engineCTX = {};
  engineCTX.ClearBackground = (tClearBackground)ClearBackground;

  engineCTX.BeginDrawing = (tBeginDrawing)BeginDrawing;
  engineCTX.EndDrawing = (tEndDrawing)EndDrawing;

  engineCTX.BeginMode2D = (tBeginMode2D)BeginMode2D;
  engineCTX.EndMode2D = (tEndMode2D)EndMode2D;

  engineCTX.DrawLine = (tDrawLine)DrawLine;
  engineCTX.DrawCircle = (tDrawCircle)DrawCircleSector;
  engineCTX.DrawEllipse = (tDrawEllipse)DrawEllipseV;
  engineCTX.DrawRectangle = (tDrawRectangle)DrawRectangleV;
  engineCTX.DrawTriangle = (tDrawTriangle)DrawTriangle;
  engineCTX.DrawPoly = (tDrawPoly)DrawPoly;
  engineCTX.DrawText = (tDrawText)DrawTextEx;

  engineCTX.GetFontDefault = (tGetFontDefault)GetFontDefault;
  engineCTX.LoadFont = (tLoadFont)LoadFont;
  engineCTX.MeasureText = (tMeasureText)MeasureTextEx;

  SceneList sceneTable = {};

  {
    FilePathList List = LoadDirectoryFilesEx(scene_path, ".dll", false);
    sceneTable = Construct_scene_table(&scene_memory, 128, scene_path, &List);
    UnloadDirectoryFiles(List);
  }

  AddScene(&sceneTable, (char *)"uiScene", &active_scene_memory);
  AddScene(&sceneTable, (char *)"uiScene", &active_scene_memory);
  AddScene(&sceneTable, (char *)"uiScene", &active_scene_memory);
  
  while (!WindowShouldClose()) {

    CheckForReload(scene_path, &sceneTable, &scene_memory);

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
