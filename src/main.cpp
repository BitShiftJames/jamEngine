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

bool CheckLibraries(SceneList *scene_list, FilePathList *file_path_list) {
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

struct src_file_list {
  u32 source_file_count;
  u64 *mod_time_list;
  char **src_path;
};

src_file_list Construct_source_list(FilePathList *List, memoryArena *src_file_memory) {

  memset(src_file_memory->memory, 0, src_file_memory->Used);

  src_file_list src_files = {};
  src_files.source_file_count = List->count;
  src_files.src_path = PushArray(src_file_memory, src_files.source_file_count, char *);
  src_files.mod_time_list = PushArray(src_file_memory, src_files.source_file_count, u64);
  for (u32 Index = 0; Index < src_files.source_file_count; Index++) {
    u32 path_length = StringLength(List->paths[Index]) + 1;
    src_files.src_path[Index] = PushArray(src_file_memory, path_length, char);
    TextCopy(src_files.src_path[Index], List->paths[Index]);

    src_files.mod_time_list[Index] = GetFileModTime(src_files.src_path[Index]);
  }

  return src_files;
}

void CheckSrcFiles(char *src_path, char *build_path, src_file_list *source_files, memoryArena *src_file_memory) {
  FilePathList List = LoadDirectoryFilesEx(src_path, ".cpp", false);

  if (source_files->source_file_count != List.count) {

    // FIXME: Repeat code turn to function?
    *source_files = Construct_source_list(&List, src_file_memory);

    if (!system(build_path)) {
      // TODO[Error handling];
    } 
    return;
  }

  for (u32 Index = 0; Index < source_files->source_file_count; Index++) {
    if (!TextEqual(source_files->src_path[Index], List.paths[Index])) {

      // FIXME: Repeat code turn to function?
      *source_files = Construct_source_list(&List, src_file_memory);

      if (!system(build_path)) {
        // TODO[Error handling];
      } 
      return;
    }

    if (source_files->mod_time_list[Index] != GetFileModTime(List.paths[Index])) {

      // FIXME: Repeat code turn to function?
      *source_files = Construct_source_list(&List, src_file_memory);

      if (!system(build_path)) {
        // TODO[Error handling];
      } 
      return;
    }
  }

  UnloadDirectoryFiles(List);
};

void CheckForReload(char *scene_path, SceneList *sceneTable, memoryArena *sceneMemory) {

  FilePathList List = LoadDirectoryFilesEx(scene_path, ".dll", false);

  if (List.count == 0) {
    return;
  }
  
  // TOOD[Timing]: Once I have a good timer system I need to set a function on this check
  // to debounce because the build time is not instantaneous
  if (CheckLibraries(sceneTable, &List)) {

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
  active_scene_memory.memory = MemAlloc(active_scene_memory.Size);

  memoryArena src_file_memory = {};
  src_file_memory.Size = Megabytes(1);
  src_file_memory.memory = MemAlloc(src_file_memory.Size);

  memoryArena scene_path_memory = {};
  scene_path_memory.Size = Kilobytes(3);
  scene_path_memory.memory = MemAlloc(scene_path_memory.Size);

  char *scene_path;
  char *src_path;
  char *build_file_path;
  SceneList sceneTable = {};
  src_file_list srcList = {};
  {
    char *working_directory = (char *)GetWorkingDirectory();
    {
      FilePathList List = LoadDirectoryFilesEx(working_directory, ".bat", true);
      if (List.count == 1) {
        int Text_Length = 0;
        build_file_path = PushArray(&scene_path_memory, 2048, char);
        TextAppend(build_file_path, "cd ", &Text_Length);

        TextAppend(build_file_path, GetDirectoryPath(List.paths[0]), &Text_Length);

        TextAppend(build_file_path, " && ", &Text_Length);

        TextAppend(build_file_path, List.paths[0], &Text_Length);
      } else {
        // Error handling.
      }
      UnloadDirectoryFiles(List);

    }

    int text_length = StringLength(working_directory);
    TextAppend(working_directory, "\\jamScenes", &text_length);

    src_path = PushArray(&scene_path_memory, StringLength(working_directory) + 1, char);
    TextCopy(src_path, working_directory);

    text_length = StringLength(working_directory);
    TextAppend(working_directory, "\\compiled_scenes", &text_length);

    scene_path = PushArray(&scene_path_memory, StringLength(working_directory) + 1, char);
    TextCopy(scene_path, working_directory);

    {
      FilePathList List = LoadDirectoryFilesEx(src_path, ".cpp", false);
      srcList = Construct_source_list(&List, &src_file_memory);
      UnloadDirectoryFiles(List);
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
      FilePathList List = LoadDirectoryFilesEx(scene_path, ".dll", false);
      sceneTable = Construct_scene_table(&scene_memory, 128, scene_path, &List);
      UnloadDirectoryFiles(List);
    }

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

  AddScene(&sceneTable, (char *)"uiScene", &active_scene_memory);
  AddScene(&sceneTable, (char *)"uiScene", &active_scene_memory);
  AddScene(&sceneTable, (char *)"uiScene", &active_scene_memory);
  
  while (!WindowShouldClose()) {

    CheckSrcFiles(src_path, build_file_path, &srcList, &src_file_memory);
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
