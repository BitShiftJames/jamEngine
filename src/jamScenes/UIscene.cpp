
#include "../jamLibrary/jamScene.h"
#include "../jamLibrary/RayAPI.h"

#if 0
#include "../jamLibrary/jamUI.h"

struct item_slot {
  jam_rect2 dim;
  Texture2D *item_slot_texture;
};

struct Inventory_UI_data {
   u32 player_slot_count;
   u32 storage_slot_count;

   item_slot player_item_slots[MAX_INVENTORY_SLOTS];
   item_slot storage_item_slots[MAX_INVENTORY_SLOTS];
};

void push_item_slot(u32 *slot_count, item_slot *item_slot_list, v2 pos, Texture2D *item_slot_texture, container_information *containerData, v2 ScreenSize) {
  if (slot_count && item_slot_list) {
    s32 slot = (*slot_count)++;
    
    item_slot_list[slot].dim.Min = pos * ScreenSize;
    item_slot_list[slot].item_slot_texture = item_slot_texture;

    if (containerData) {
      item_slot_list[slot].dim.Min = containerData->container_pos;
    }

    item_slot_list[slot].dim.Max.x = item_slot_texture->width + item_slot_list[slot].dim.Min.x;
    item_slot_list[slot].dim.Max.y = item_slot_texture->height + item_slot_list[slot].dim.Min.y;
    
    Expand_container(containerData, item_slot_list[slot].dim.Max);
    
  }

  mainGameUI_data *uiData = (mainGameUI_data *)self->data;

  for (u32 i = 0; i < uiData->invUI.player_slot_count; i++) {
    item_slot *curritem_slot = &uiData->invUI.player_item_slots[i];

    DrawTexture(*curritem_slot->item_slot_texture, curritem_slot->dim.x, curritem_slot->dim.y, WHITE);
  }

  mainGameUI_data *uiData = PushStruct(self->arena, mainGameUI_data);



  uiData->item_slot = LoadTexture("../assets/item_slot.png");
  uiData->item_icons = LoadTexture("../assets/itemsheet.png");

  uiData->playerInventory = PushStruct(self->arena, Inventory_information);
  uiData->storageInventory = PushStruct(self->arena, Inventory_information);

  uiData->playerInventory->DisplaySlots = 9;

  EndContainer(&uiData->ui);
  self->data = uiData;
};

// UI scene function
struct mainGameUI_data {
  UI_data ui;
  Inventory_UI_data invUI;

  Inventory_information *playerInventory;
  Inventory_information *storageInventory;

  Texture2D item_slot;
  Texture2D item_icons;
};

#endif


extern "C" __declspec(dllexport) void scene_update(struct Scene *self) {
  int a = 2;
}

extern "C" __declspec(dllexport) void scene_render(struct Scene *self, RayAPI *engineCTX) {
  engineCTX->ClearBackground(Color_{255, 255, 255, 255});
}

extern "C" __declspec(dllexport) void scene_onEnter(struct Scene *self) {
  int a = 2;
}

extern "C" __declspec(dllexport) void scene_onExit(struct Scene *self) {
  int a = 2;
}
