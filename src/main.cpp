#include "jamCollision.h"
#include "jamInventory.h"
#include "jamMath.h"

#include "menuScene.cpp"

#define PERMISSION 0
#if PERMISSION
// a profiler from Handmade hero (Day 176-179). 
// I got permission but I probably won't include it in the code still.
#include "jamDebug.h"
#else
#define TIMED_BLOCK __noop
#endif

#include "raylib.h"

#include "jamTypes.h"
#include "jamScene.h"

// TODO[Scenes]: These need to be hot reloaded.

#include <string.h>

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



int main() {
  u32 flags = FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT;// FLAG_WINDOW_TOPMOST | FLAG_WINDOW_UNDECORATED;
  SetConfigFlags(flags);
  // passing 0 makes raylib window the size of the screen.
  InitWindow(0, 0, "Restarting from scratch");
  SetTraceLogLevel(LOG_ALL);

  SetTargetFPS(30);

  s32 ScreenWidth = GetScreenWidth();
  s32 ScreenHeight = GetScreenHeight();

  CursorObject global_cursor = {};
  global_cursor.texture = LoadTexture("../assets/Cursor.png");

  global_cursor.layer1 = {0, 0, (f32)global_cursor.texture.width, (f32)((u32)(global_cursor.texture.height / 3))};
  global_cursor.layer2 = {0, CURSOR_SIDE, (f32)global_cursor.texture.width, (f32)((u32)(global_cursor.texture.height / 3))};
  global_cursor.layer3 = {0, CURSOR_SIDE * 2, (f32)global_cursor.texture.width, (f32)((u32)(global_cursor.texture.height / 3))};

  Cursor_inventory_information CursorInventory = {};
  CursorInventory.dest_rect = JamRectMinDim(v2{0, 0}, v2{16, 16});
  global_cursor.Inventory = CursorInventory;

  memoryArena Scene_arena = {};
  // different thread maybe.
  Scene_arena.memory = MemAlloc(Gigabytes(2));
  Scene_arena.Size = Gigabytes(2);
  
  memoryArena Temp_arena = {};
  // different thread, maybe.
  Temp_arena.memory = MemAlloc(Gigabytes(1));
  Temp_arena.Size = Gigabytes(1);

  memoryArena Aux_arena = {};
  Aux_arena.memory = MemAlloc(Megabytes(200));
  Aux_arena.Size = Megabytes(200);

  // these use static strings so might as well do it once and never again.
  
  const char *working_directory = GetWorkingDirectory();
  Scene currScene = {};
  Scene auxScene = {};

  currScene.arena = &Scene_arena;
  currScene.temp_arena = &Temp_arena;

  auxScene.arena = &Aux_arena; 
  auxScene.temp_arena = 0;
  {
    Vector2 MousePos = GetMousePosition();
    v2 mousePos = {MousePos.x, MousePos.y};

    currScene.MousePos = mousePos;
    currScene.ScreenSize = {(f32)GetScreenWidth(), (f32)GetScreenHeight()};
  }

  {
    s32 count = 0;
    char **split_text= TextSplit(working_directory, '\\', &count);

    char *join_text = TextJoin(split_text, count - 1, "\\");
    TextCopy(currScene.parent_directory, join_text);

    s32 text_length = TextLength(join_text);
    TextAppend(join_text, "\\saves", &text_length);
    TextCopy(currScene.save_directory, join_text);

  }
  

  InitScene(&currScene, &auxScene);

  {
    Scene mainScene = {};

    mainScene.onEnter = mainMenu_onEnter;
    mainScene.onExit = mainMenu_onExit;
    mainScene.update = mainMenu_update;
    mainScene.render = mainMenu_render;
  
    SetScene(GetCurrScene(), &mainScene); 
  }

  while (!WindowShouldClose()) {

    Vector2 MousePos = GetMousePosition();
    v2 mousePos = {MousePos.x, MousePos.y};

    currScene.MousePos = mousePos;
    currScene.ScreenSize = {(f32)GetScreenWidth(), (f32)GetScreenHeight()};

    // TODO[Scenes]: At some point there should be a scene list.
    // Where you have things like a UI scene, Timing scene, and other such things.
    // Unless I want the timers to async in which case I will need to do multi-threaded scene shannigans.
    if (currScene.update) {
      currScene.update(&currScene);
    }

    if (auxScene.update) {
      auxScene.update(&auxScene);
    }

    BeginDrawing();
      
  
      if (currScene.render) {
        currScene.render(&currScene);
      }

      if (auxScene.render) {
        auxScene.render(&auxScene);
      }
      
    EndDrawing();
    //

  }

  CloseWindow();
  return 0;
}

#if PERMISSION
  debugRecords MainDebugRecords[__COUNTER__];
  #include "jamDebug.cpp"
#endif
