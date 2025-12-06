#include "jamLibrary/jamTypes.h"
#include "jamLibrary/jamScene.h"

#include "raylib.h"

#include "platform_win32.h"

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

struct scene_table {
  u32 scene_count;
  char **scene_name;
  char **scene_path;
};

Scene Load_scene(scene_table *sceneTable, char *name) {
  Scene result = {};
  for (u32 i = 0; i < sceneTable->scene_count; i++) {
    if (TextIsEqual(sceneTable->scene_name[i], name)) {

      void *dllHandle = load_a_library(sceneTable->scene_path[i]);

      result.onEnter = (sceneOnEnter)gimme_function(dllHandle, (char *)"scene_onEnter");
      result.onExit = (sceneOnExit)gimme_function(dllHandle, (char *)"scene_onExit");
      result.update = (sceneUpdate)gimme_function(dllHandle, (char *)"scene_update");
      result.render = (sceneRender)gimme_function(dllHandle, (char *)"scene_render");

      return result;
    }
  }
  return result;
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
  
  char *working_directory = (char *)GetWorkingDirectory();

  {
    int text_length = TextLength(working_directory);
    TextAppend(working_directory, "\\jamScenes\\compiled_scenes", &text_length);
  }

  FilePathList scenes = LoadDirectoryFiles(working_directory);

  memoryArena scene_memory = {};
  scene_memory.memory = MemAlloc(Megabytes(200));
  scene_memory.Size = Megabytes(200);

  u32 scene_count = 0;
  scene_table sceneTable = {};

  sceneTable.scene_path = PushArray(&scene_memory, 128, char *);
  sceneTable.scene_name = PushArray(&scene_memory, 128, char *);

  for (u32 i = 0; i < scenes.count; i++) {
    if (IsFileExtension((scenes.paths[i]), ".dll") 
        && !TextIsEqual(GetFileNameWithoutExt(scenes.paths[i]), "raylib")) {

      s32 path_text_length = TextLength(scenes.paths[i]) + 1;
      char *currPath = PushArray(&scene_memory, path_text_length, char);
      
      s32 scene_name_text_length = TextLength(GetFileNameWithoutExt(scenes.paths[i])) + 1;
      char *scene_name_text = PushArray(&scene_memory, scene_name_text_length, char);


      TextCopy(currPath, scenes.paths[i]);
      TextCopy(scene_name_text, GetFileNameWithoutExt(scenes.paths[i]));

      sceneTable.scene_name[sceneTable.scene_count] = scene_name_text;
      sceneTable.scene_path[sceneTable.scene_count] = currPath;

      sceneTable.scene_count++;
    }
  }
  
  
  Scene test_scene = Load_scene(&sceneTable, (char *)"menuScene");

  Scene global_scene = {};
  InitScene(&global_scene);

  SetScene(&test_scene);




  while (!WindowShouldClose()) {

    BeginDrawing();
      
    EndDrawing();
    //

  }

  CloseWindow();
  return 0;
}
