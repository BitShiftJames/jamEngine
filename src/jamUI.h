#ifndef JAM_UI_H
#define JAM_UI_H

#include "jamTypes.h"
#include "jamMath.h"
#include "jamInventory.h"
#include "jamCollision.h"

// TODO: See if decoupling some components from jamEntities is worth it.
#include "jamEntities.h"

#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#if 0
#define MAX_HEALTH 400
#define HEALTH_SLOTS 40
#define HALF_HEALTH_SLOTS 20


// untracked things will typically have collision and be something that is harder to track from frame to frame.
// like inventory data.


// Horrible naming scheme is horrible in all the ways that a naming scheme can be
// without actually being abstract and abbrevated. 

struct Entity_UI_snapshot {
  healthComponent *bossHealthInformation;
  healthComponent *playerHealthInformation;

};

struct Entity_UI_data {
  healthComponent *bossHealthInformation;
  healthComponent *playerHealthInformation;

  Entity_UI_snapshot prevElements;
};

void DrawUI(Inventory_UI_data *invData , Entity_UI_data *entityData, RenderTexture2D UI_texture) {

    b32 health_change = (entityData->playerHealthInformation->Health - entityData->prevElements.playerHealthInformation->Health) ? 0 : 1;
    b32 effect_change = (entityData->playerHealthInformation->EffectFlags - entityData->prevElements.playerHealthInformation->EffectFlags) ? 0 : 1;

    // TODO[UI]: See about doing a little bit more with the tracking stuff. 
    // Maybe more granular tracking on the inventory data so that the redraw get's to do less work.
    // like checking if the collision should be recalculated. 
    entityData->prevElements = {
      entityData->bossHealthInformation,
      entityData->playerHealthInformation 
    };

    if (health_change || effect_change || invData->playerInventory->dirty || invData->storageInventory->dirty) {
      BeginTextureMode(UI_texture);
        invData->player_collision_count = 0;
        invData->storage_collision_count = 0;
        ClearBackground(Color{0, 0, 0, 0});
      
        for (u32 i = 0; i < invData->playerInventory->DisplaySlots; i++) {
          u32 slotY = i % invData->playerInventory->Row;
          u32 slotX = i / invData->playerInventory->Row;
          
          Rectangle destRect = {(f32)(slotX * (invData->playerInventory->Size * 1.2) + 20), (f32)(slotY * (invData->playerInventory->Size * 1.2) + 200), 
                                (f32)invData->playerInventory->Size, (f32)invData->playerInventory->Size};
          
          DrawRectangleRounded(destRect, .3f, 10, Color{100, 0, 255, 128});
          invData->playerInvCollision[invData->player_collision_count++] = JamRectMinDim(destRect);
          if (invData->playerInventory->storage[i].HasItem) {

            DrawTexturePro(invData->item_icons, JamToRayRect(invData->playerInventory->storage[i].item_in_me.SourceRect), destRect, Vector2{0, 0}, 0.0f, WHITE);
          }
        }

        for (u32 i = 0; i < invData->storageInventory->DisplaySlots; i++) {
          u32 slotY = i % invData->storageInventory->Row;
          u32 slotX = i / invData->storageInventory->Row;
          
          Rectangle destRect = {(f32)(-(slotX * (invData->playerInventory->Size * 1.2)) + (GetScreenWidth() - 80)), (f32)(slotY * (invData->playerInventory->Size * 1.2) + 200), 
                                (f32)invData->playerInventory->Size, (f32)invData->playerInventory->Size};
          DrawRectangleRounded(destRect, .3f, 10, Color{100, 0, 255, 128});
          invData->StorageInvCollision[invData->storage_collision_count++] = JamRectMinDim(destRect);
          if (invData->storageInventory->storage[i].HasItem) {

            DrawTexturePro(invData->item_icons, JamToRayRect(invData->storageInventory->storage[i].item_in_me.SourceRect), destRect, Vector2{0, 0}, 0.0f, WHITE);
          }
        }

        u32 max_count = (u32)(((f32)entityData->playerHealthInformation->Health / (f32)MAX_HEALTH) * HEALTH_SLOTS);
        for (u32 i = 0; i < max_count; i++) {
          u32 Y = i / HALF_HEALTH_SLOTS;
          u32 X = i % HALF_HEALTH_SLOTS;
          DrawRectangle( 30 + (X * 27), (Y * 30) + 30, 25, 20, RED);
        }
        
      invData->storageInventory->dirty = false;
      invData->playerInventory->dirty = false;

      EndTextureMode();
    }

  DrawTexturePro(UI_texture.texture, Rectangle{0.0f, 0.0f, (f32)UI_texture.texture.width, -(f32)UI_texture.texture.height}, Rectangle{0.0f, 0.0f, (f32)UI_texture.texture.width, (f32)UI_texture.texture.height}, Vector2{0.0f, 0.0f}, 0.0f, WHITE);

  // TODO[UI]: The cursor really isn't a UI element it should be a part of the renderer. 
  // Cause it would be nice to just make one texture for the cursor and only update it if the setting is changed.
  // and then layer that with the item data or something.
  
}
#endif

struct Buttons {
  jam_rect2 dim;

  Color text_color;
  Color hover_color;

  s32 size;
  char *text;

  void (*callback)(void *userdata);
  void *userdata;
  u32 menu_change;
};

#define WORLD_NAME_MAX_CHAR_COUNT 512

struct File_box {
  jam_rect2 dim;

  Color text_color;
  Color hover_color;
  Color deletion_color;

  b32 deletion_flag;

  s32 size;
  char text[WORLD_NAME_MAX_CHAR_COUNT];
  char *file_path;
};

struct Text_box {
  Color text_color;

  v2 pos;

  b32 deletion_flag;

  s32 size;
  char *text;
};

enum container_types {
  menu_flag_v_box = 0,
  menu_flag_h_box,
};

struct container_information {
  bool in_container;
  u16 padding;
  container_types container_type;
  v2 container_pos;
};

struct UI_data {
  container_information containerData;

  u32 buttons_count;
  Buttons buttons[32];

  u32 file_count;
  File_box file_boxes[32];

  u32 text_box_count;
  Text_box text_boxes[32];
};

static void StartContainer(UI_data *data, v2 pos, u16 padding, container_types container_type, v2 ScreenSize) {
  data->containerData.in_container = true;
  data->containerData.padding = padding;
  data->containerData.container_pos = pos * ScreenSize;
  data->containerData.container_type = container_type;
}

static void EndContainer(UI_data *data) {
  data->containerData.in_container = false;
}

static void Expand_container(container_information *container_info, v2 Max) {
  if (container_info) {
    switch (container_info->container_type) {
      case menu_flag_v_box: {
        container_info->container_pos.y = Max.y + container_info->padding;
      } break;
      case menu_flag_h_box: {
        container_info->container_pos.x = Max.x + container_info->padding;
      } break;
      default: {
        // Error Handling.
      } break;
    }
  } else {
    return;
  }
}

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
    
    if (data->containerData.in_container) {
      // FIXME: Container logic better also the button dimension code mix be collapse able.
      currButton->dim.Min = data->containerData.container_pos;
      currButton->dim.Max.x = currButton->dim.Min.x + MeasureText(text, size);
      currButton->dim.Max.y = currButton->dim.Min.y + size;

      if (data->containerData.container_type == menu_flag_v_box) {
        data->containerData.container_pos.y = currButton->dim.Max.y + data->containerData.padding;
      } 

    } else {
      currButton->dim.Min = pos * ScreenSize;
      currButton->dim.Max.x = currButton->dim.Min.x + MeasureText(text, size);
      currButton->dim.Max.y = currButton->dim.Min.y + size;
    }
    
    data->buttons_count++;
  }
}

void push_file_box(UI_data *data, s32 size, v2 pos, char *text, char *file_path, Color textColor, Color hoverColor, Color DeletionColor, v2 ScreenSize) {

  Assert(pos.x > 1.0f || pos.y > 1.0f)

  if (data->file_count < ArrayCount(data->buttons)) {
    File_box *currFile_box = &data->file_boxes[data->file_count];

    TextCopy(currFile_box->text, text);
    currFile_box->file_path = file_path;
    currFile_box->size = size;

    currFile_box->text_color = textColor;
    currFile_box->hover_color = hoverColor;
    currFile_box->deletion_color = DeletionColor;
    
    if (data->containerData.in_container) {
      currFile_box->dim.Min = data->containerData.container_pos;
      currFile_box->dim.Max.x = currFile_box->dim.Min.x + MeasureText(text, size);
      currFile_box->dim.Max.y = currFile_box->dim.Min.y + size;

      if (data->containerData.container_type == menu_flag_v_box) {
        data->containerData.container_pos.y = currFile_box->dim.Max.y + data->containerData.padding;
      } 

    } else {
      currFile_box->dim.Min = pos * ScreenSize;
      currFile_box->dim.Max.x = currFile_box->dim.Min.x + MeasureText(text, size);
      currFile_box->dim.Max.y = currFile_box->dim.Min.y + size;
    }

    data->file_count++;
  };
};

void push_text_box(UI_data *data, s32 size, char *text, v2 pos, Color textColor, v2 ScreenSize) {

  Assert(pos.x > 1.0f || pos.y > 1.0f)

  if (data->text_box_count < ArrayCount(data->text_boxes)) {
    Text_box *currText_box = &data->text_boxes[data->text_box_count];

    currText_box->size = size;
    currText_box->text = text;
    currText_box->text_color = textColor;

    if (data->containerData.in_container) {
    } else {
      currText_box->pos = pos * ScreenSize;
    }

    data->text_box_count++;
  }
}


#endif
