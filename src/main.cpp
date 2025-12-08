#include "jamLibrary/jamText.h"
#include "jamLibrary/jamTypes.h"
#include "jamLibrary/jamScene.h"

#include "raylib.h"


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
  scene_memory.Size = Megabytes(20);
  scene_memory.memory = MemAlloc(scene_memory.Size);

  char *scene_path;
  {
    char *working_directory = (char *)GetWorkingDirectory();
    int text_length = StringLength(working_directory);
    TextAppend(working_directory, "\\jamScenes\\compiled_scenes", &text_length);

    scene_path = PushArray(&scene_memory, StringLength(working_directory), char);
    TextCopy(scene_path, working_directory);
  }


  SceneList sceneTable = Construct_scene_table(&scene_memory, 128, scene_path);

  AddScene(&sceneTable, (char *)"menuScene", &scene_memory);
  AddScene(&sceneTable, (char *)"uiScene", &scene_memory);
  AddScene(&sceneTable, (char *)"menuScene", &scene_memory);
  
  while (!WindowShouldClose()) {

    BeginDrawing();
      
    EndDrawing();
    //

  }

  CloseWindow();
  return 0;
}
