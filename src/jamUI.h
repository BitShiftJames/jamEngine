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

#define CURSOR_SIDE 32
#define CURSOR_HALF_SIDE 16

#define MAX_HEALTH 400
#define HEALTH_SLOTS 40
#define HALF_HEALTH_SLOTS 20


// untracked things will typically have collision and be something that is harder to track from frame to frame.
// like inventory data.

struct Inventory_UI_data {
  Inventory_information *playerInventory;
  Inventory_information *storageInventory;

  // TODO: Smarter UI collision.
  u32 player_collision_count;
  u32 storage_collision_count;
  jam_rect2 playerInvCollision[MAX_INVENTORY_SLOTS];
  jam_rect2 StorageInvCollision[MAX_INVENTORY_SLOTS];

  Texture2D item_icons;
};

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
          // USELESS FUCKING LSP.
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
