#include "raylib.h"
#include "jamMath.h"
#include "jamTypes.h"
#include "jamScene.h"
#include "jamCollision.h"
#include "jamUI.h"


#include <cstdlib>
#include <cstring>

#include "mainGameScene.cpp"

#include "safe_delete.h"

#define MAX_ID_COUNT 256

enum menus {
  Main_menu = 0,
  Start_menu,
  Setting_menu,
  Tooling_menu,
};

struct UI_data {
  u32 buttons_count;
  Buttons buttons[32];

  u32 file_count;
  File_box file_boxes[32];

  u32 text_box_count;
  Text_box text_boxes[32];
};

struct menuChange;
// terrible_naming_scheme001.exe
struct mainmenu_data {
  u32 ui_count;
  s8 current_ui;
  UI_data *uiData;

  FilePathList list;
  menuChange *menuChanges;

  b32 delete_mode;
};

// I can't think of a way to do this with better scope.
// other than doing a void * list of args but I don't 
// know how often I need to pass more data to the callback then just one

struct menuChange {
  mainmenu_data *arg1;
  s8 arg2;
};

// TODO[UI]: Make hoverColor option.
void push_buttons(UI_data *data, s32 size, char *text, v2 pos, Color textColor, Color hoverColor, 
                  void (*callback)(void *userdata), void *userdata, v2 ScreenSize) {
  Assert(pos.x > 1.0f || pos.y > 1.0f)

  if (data->buttons_count < ArrayCount(data->buttons)) {
    Buttons *currButton = &data->buttons[data->buttons_count];

    currButton->size = size;
    currButton->text = text;
    currButton->text_color = textColor;
    currButton->hover_color = hoverColor;
    currButton->callback = callback;
    currButton->userdata = userdata;

    currButton->dim.Min = pos * ScreenSize;
    currButton->dim.Max.x = currButton->dim.Min.x + MeasureText(text, size);
    currButton->dim.Max.y = currButton->dim.Min.y + size;
    
    data->buttons_count++;
  }
}

void push_file_box(UI_data *data, s32 size, v2 pos, char *text, char *file_path, Color textColor, Color hoverColor, Color DeletionColor, v2 ScreenSize) {

  Assert(pos.x > 1.0f || pos.y > 1.0f)

  if (data->file_count < ArrayCount(data->buttons)) {
    File_box *currFile_box = &data->file_boxes[data->file_count];

    currFile_box->text = text;
    currFile_box->file_path = file_path;
    currFile_box->size = size;

    currFile_box->text_color = textColor;
    currFile_box->hover_color = hoverColor;
    currFile_box->deletion_color = DeletionColor;

    currFile_box->dim.Min = pos * ScreenSize;
    currFile_box->dim.Max.x = currFile_box->dim.Min.x + MeasureText(text, size);
    currFile_box->dim.Max.y = currFile_box->dim.Min.y + size;

    data->file_count++;
  };
};

void push_text_box(UI_data *data, s32 size, char *text, v2 pos, Color textColor, v2 ScreenSize) {

  Assert(pos.x > 1.0f || pos.y > 1.0f)

  if (data->text_box_count < ArrayCount(data->text_boxes)) {
    Text_box *currText_box = &data->text_boxes[data->text_box_count];

    currText_box->size = size;
    currText_box->text = text;
    currText_box->pos = pos * ScreenSize;
    currText_box->text_color = textColor;

    data->text_box_count++;
  }
}

void QuitIT(void *userData) {
  exit(0);
}

void DeletionMode(void *userData) {
  mainmenu_data *menuData = (mainmenu_data *)userData;

  menuData->delete_mode = true;
}

void ChangeMenu(void *userData) {
  menuChange *menuChangeData = (menuChange *)userData;
  mainmenu_data *menuData = menuChangeData->arg1;
  s8 next_ui = menuChangeData->arg2;

  if (next_ui > -1) {
    menuData->current_ui = next_ui;
  }

}

void StartPlaying(File_box *file) {

  // FIXME: Implict behavior. Have transitional memory.
  Scene *currScene = GetScene();

  File_box *temp_file_box = PushStruct(currScene->temp_arena, File_box);

  *temp_file_box = *file;

  currScene->data = temp_file_box;

  Scene playScene = {};
  playScene.update = mainGame_update;
  playScene.render = mainGame_render;
  playScene.onEnter = mainGame_onEnter;
  playScene.onExit = mainGame_onExit;

  SetScene(&playScene);
}

void WorldGeneration(void *userData) {
  Scene *sceneData = (Scene *)userData;

  memoryArena *arena = sceneData->temp_arena;
  world *global_world = PushStruct(arena, world);

  global_world->Width = 500;
  global_world->Height = 300;
  global_world->TileSize = 16;
  global_world->gravity_constant = 50;

  global_world->map = PushArray(arena, global_world->Width * global_world->Height, tile);

  v2 spawn_location = {};
  for (u32 tileY = 0; tileY < global_world->Height; tileY++) {
    for (u32 tileX = 0; tileX < global_world->Width; tileX++) {
      tile CurrentTile = getTile(global_world, tileX, tileY);
      if (tileY > global_world->Height /2) {
          if (tileX > (global_world->Width / 2) && tileX < (global_world->Width / 2) + 40) {
          } else {
            CurrentTile.type = 43;
          }
          CurrentTile.light = packR4G4B4AF(0, 0, 0);
      } else {
      }

      if (tileY > global_world->Height / 1.2) { 
            CurrentTile.type = 43;
      }

      if (CurrentTile.type == 0) {
        CurrentTile.light = packR4G4B4AF(15, 15, 15);
      }

      if (tileX > global_world->Width - 40) {
        CurrentTile.type = 1;
          CurrentTile.light = packR4G4B4AF(15, 0, 0);
      }

      if (tileX == global_world->Width / 2 && tileY == (global_world->Height / 2) - 3) {
        spawn_location = {(f32)tileX * global_world->TileSize, (f32)tileY * global_world->TileSize};
      }

      global_world->map[tileY * global_world->Width + tileX] = CurrentTile;
    }
  }
  
  SaveFileData("../saves/test1.sav", arena->memory, arena->Used);
  
  ClearAnArena(arena);

};

static inline void Reconstruct_Start_Menu(Scene *self, mainmenu_data *menuData) {
  UI_data *startMenudata = &menuData->uiData[Start_menu];
  menuChange *mainMenuChange = &menuData->menuChanges[Main_menu];
  
  startMenudata->file_count = 0;
  // TODO[World Generation]: Implement
  for (u32 i = 0; i < menuData->list.count; i++) {
    push_file_box(startMenudata, 
                  40, // text size
                  {0.5f, 0.3f + (i * .05f)}, // position
                  menuData->list.paths[i], // text
                  menuData->list.paths[i],  // file_path
                  WHITE, // text color
                  YELLOW, // hover color
                  RED,
                  self->ScreenSize); // deletion color
  }
}

static void DeleteAFile(char *file_path) {
  s32 Length = TextLength(file_path);
  if (Length < MAX_FILE_PATH - 1) {
    file_path[Length + 1] = '\0';
    recycle_delete(file_path);
  } else {
    // Fail due to running out of space.
  }
}

static void mainMenu_update(struct Scene *self) {
  mainmenu_data *menuData = (mainmenu_data *)self->data;

  FilePathList checkList = LoadDirectoryFiles(self->save_directory);
  b32 change_in_file_path = false;

  if (checkList.count != menuData->list.count) {
    change_in_file_path = true;
  } else {
    for (u32 i = 0; i < menuData->list.count; i++) {
      if (!TextIsEqual(menuData->list.paths[i], checkList.paths[i])) {
        change_in_file_path = true;
      }
    }
  }

  if (change_in_file_path) {
    menuData->list = checkList;
    Reconstruct_Start_Menu(self, menuData);
  }

  // tiny function to inverse the if statement?
  s32 button_count = menuData->uiData[menuData->current_ui].buttons_count;
  if (button_count) {
    for (u32 i = 0; i < button_count; i++) {
      Buttons *currButton = &menuData->uiData[menuData->current_ui].buttons[i];
    
      if (PointInRect(currButton->dim, self->MousePos) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {

          if (currButton->callback) {
            currButton->callback(currButton->userdata);
          }
      }

    }
  }

  s32 file_count = menuData->uiData[menuData->current_ui].file_count;
  if (file_count) {
    for (u32 i = 0; i < file_count; i++) {
      File_box *currFile_box = &menuData->uiData[menuData->current_ui].file_boxes[i];

      if (PointInRect(currFile_box->dim, self->MousePos) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (menuData->delete_mode) {
          // TODO[Polish]: Make it so you can do a mass selection of worlds and delete them all at once with one are you sure prompt?
          DeleteAFile(currFile_box->file_path);
          menuData->delete_mode = false;
        } else {
          StartPlaying(currFile_box);
        }
      }

    }
  }

}

static void mainMenu_render(struct Scene *self) {
  ClearBackground(SKYBLUE);
  mainmenu_data *menuData = (mainmenu_data *)self->data;
  UI_data *uiData = &menuData->uiData[menuData->current_ui];

  if (uiData->buttons_count) {
    for (u32 i = 0; i < uiData->buttons_count; i++) {
      Buttons *currButton = &uiData->buttons[i];

      Color textColor = currButton->text_color;

      if (PointInRect(currButton->dim, self->MousePos)) {
        textColor = currButton->hover_color;
      }
      
      DrawText(currButton->text, 
               currButton->dim.x, 
               currButton->dim.y, 
               currButton->size, 
               textColor);

      DrawCircle(self->MousePos.x, self->MousePos.y, 1.0f, PURPLE);

    }
  }

  if (uiData->text_box_count) {
    for (u32 i = 0; i < uiData->text_box_count; i++) {
      Text_box *currTextbox = &uiData->text_boxes[i];
      
      DrawText(currTextbox->text, 
               currTextbox->pos.x, 
               currTextbox->pos.y, 
               currTextbox->size, 
               currTextbox->text_color);

    }
  }

  if (uiData->file_count) {
    for (u32 i = 0; i < uiData->file_count; i++) {
      File_box *currFilebox= &uiData->file_boxes[i];
      Color textColor = currFilebox->text_color;
      
      if (PointInRect(currFilebox->dim, self->MousePos)) {
        if (menuData->delete_mode) {
          textColor = currFilebox->deletion_color;
        } else {
          textColor = currFilebox->hover_color;
        }
      }
      
      DrawText(currFilebox->text, 
               currFilebox->dim.x, 
               currFilebox->dim.y, 
               currFilebox->size, 
               textColor);

    }
  }
  
}
static void mainMenu_onEnter(struct Scene *self) {

  memset(self->arena->memory, 0, self->arena->Size);
  self->arena->Used = 0;
  
  mainmenu_data *menuData = PushStruct(self->arena, mainmenu_data);
  menuData->ui_count = 4; 
  menuData->uiData = PushArray(self->arena, menuData->ui_count, UI_data);
  menuData->menuChanges = PushArray(self->arena, menuData->ui_count, menuChange);
  
  menuData->list = LoadDirectoryFiles(self->save_directory);

  menuChange *mainMenuChange = &menuData->menuChanges[Main_menu];
  mainMenuChange->arg1 = menuData;
  mainMenuChange->arg2 = (s8)Main_menu;

  menuChange *startMenuChange = &menuData->menuChanges[Start_menu];
  startMenuChange->arg1 = menuData;
  startMenuChange->arg2 = (s8)Start_menu;

  menuChange *settingChange = &menuData->menuChanges[Setting_menu];
  settingChange->arg1 = menuData;
  settingChange->arg2 = (s8)Setting_menu;

  menuChange *toolingChange = &menuData->menuChanges[Tooling_menu];
  toolingChange->arg1 = menuData;
  toolingChange->arg2 = (s8)Tooling_menu;


  UI_data *mainMenudata = &menuData->uiData[Main_menu];
  push_text_box(mainMenudata, 120, (char *)"Working-title", v2{.2f, .1f}, WHITE, self->ScreenSize);
  push_text_box(mainMenudata, 60, (char *)"\"that works\"", v2{.25f, .22f}, WHITE, self->ScreenSize);
  push_buttons(mainMenudata, 40, (char *)"Start", {.2f, .3f}, WHITE, YELLOW, ChangeMenu, startMenuChange, self->ScreenSize);
  push_buttons(mainMenudata, 40, (char *)"Setting", {.2f, .4f}, WHITE, YELLOW, ChangeMenu, settingChange, self->ScreenSize);
  push_buttons(mainMenudata, 40, (char *)"Tooling", {.2f, .5f}, WHITE, YELLOW, ChangeMenu, toolingChange, self->ScreenSize);
  push_buttons(mainMenudata, 40, (char *)"Quitting", {.2f, .6f}, WHITE, YELLOW, QuitIT, 0, self->ScreenSize);

  UI_data *startMenudata = &menuData->uiData[Start_menu];
  push_buttons(startMenudata, 40, (char *)"Create New World", {.2f, .3f}, WHITE, YELLOW, WorldGeneration, self, self->ScreenSize);
  push_buttons(startMenudata, 40, (char *)"Delete A World", {.2f, .34f}, WHITE, YELLOW, DeletionMode, menuData, self->ScreenSize);
  push_buttons(startMenudata, 40, (char *)"Back", {.2f, .38f}, WHITE, YELLOW, ChangeMenu, mainMenuChange, self->ScreenSize);
  Reconstruct_Start_Menu(self, menuData);

  UI_data *settingsMenudata = &menuData->uiData[Setting_menu];
  push_buttons(settingsMenudata, 40, (char *)"Back", {.2f, .5f}, WHITE, YELLOW, ChangeMenu, mainMenuChange, self->ScreenSize);

  UI_data *toolingMenudata = &menuData->uiData[Tooling_menu];
  push_buttons(toolingMenudata, 40, (char *)"Back", {.2f, .5f}, WHITE, YELLOW, ChangeMenu, mainMenuChange, self->ScreenSize);
  
  self->data = menuData;
}

static void mainMenu_onExit(struct Scene *self) {
}
