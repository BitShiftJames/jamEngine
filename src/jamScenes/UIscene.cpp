
#include "../jamLibrary/jamScene.h"
#include "../jamLibrary/RayAPI.h"
#include "../jamLibrary/jamMath.h"
#include "../jamLibrary/jamUI.h"

#include <cmath>
#include <cstdio>
#include <cstring>

#if 0

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


#define PIE 3.14159

v4 ZeroQuaternion() {
  v4 result = {};

  f32 cosine_value = cosf(0.0f/2.0f);

  f32 sine_value = sinf(0.0f/2.0f);
  v3 Axis {};
  Axis.x *= sine_value;
  Axis.y *= sine_value;
  Axis.z *= sine_value;


  result.x = Axis.x;
  result.y = Axis.y;
  result.z = Axis.z;
  result.w = cosine_value;

  return result;
};

v4 NormalizeQuaternion(v4 Quaternion) {
  v4 result = {};

  f32 length = sqrtf(Quaternion.x * Quaternion.x + Quaternion.y * Quaternion.y + Quaternion.z * Quaternion.z);
  if (length == 0.0f) {
    length = 1.0f;
  }
  f32 Inverse_length = 1.0f / length;
  result.x = Quaternion.x * Inverse_length;
  result.y = Quaternion.y * Inverse_length;
  result.z = Quaternion.z * Inverse_length;
  result.w = Quaternion.w;
  Assert(result.w < 1.0f);

  return result; 
};

v4 AngleAxisQuaternion(f32 AngleDegrees, v3 Axis) {
  v4 result = {};

  f32 angleRadians = AngleDegrees * (PIE / 180);

  f32 cosine_value = cosf(angleRadians/2.0f);
  f32 sine_value = sinf(angleRadians/2.0f);
  f32 length = sqrtf(Axis.x * Axis.x + Axis.y * Axis.y + Axis.z * Axis.z);
  if (length == 0.0f) {
    length = 1.0f;
  }
  f32 Inverse_length = 1.0f / length;
  Axis.x *= Inverse_length;
  Axis.y *= Inverse_length;
  Axis.z *= Inverse_length;

  Axis.x *= sine_value;
  Axis.y *= sine_value;
  Axis.z *= sine_value;


  result.x = Axis.x;
  result.y = Axis.y;
  result.z = Axis.z;
  result.w = cosine_value;

  return result;
}

v4 InverseQuaternion(v4 Q) {
  v4 result = {};
  f32 x = -Q.x;
  f32 y = -Q.y;
  f32 z = -Q.z;
  f32 w = Q.w;
  f32 SqMag = Q.x * Q.x + Q.y * Q.y + Q.z * Q.z + Q.w * Q.w;
  
  result.x = x / SqMag;
  result.y = y / SqMag;
  result.z = z / SqMag;
  result.w = w / SqMag;

  return result;
}

v4 multiplyQuaternion(v4 A, v4 B) {
  v4 result = {};

  result.x = A.w * B.x + A.x * B.w + A.y * B.z - A.z * B.y;
  result.y = A.w * B.y - A.x * B.z + A.y * B.w + A.z * B.x;
  result.z = A.w * B.z + A.x * B.y - A.y * B.x + A.z * B.z;
  result.w = A.w * B.w - A.x * B.x - A.y * B.y - A.z * B.z;

  //result.w = A.x*B.x - A.y*B.y - A.z*B.z - A.w*B.w;
  //result.x = A.x*B.y + A.y*B.x + A.z*B.w - A.w*B.z;
  //result.y = A.x*B.z - A.y*B.w + A.z*B.x + A.w*B.y;
  //result.z = A.x*B.z + A.y*B.z - A.z*B.y + A.w*B.x;

  return result;
};

v3 VectorRotate(v3 origin, v3 forward, v4 q) {
  v3 result = {};
  
  v3 v = forward - origin;

  v3 u = {q.x, q.y, q.z};
  f32 s = q.w;
  
  result.x = 2.0f * dot_v3(u, v) * u.x + (s * s - dot_v3(u, u)) * v.x + 2.0f * s * cross(u, v).x;
  result.y = 2.0f * dot_v3(u, v) * u.y + (s * s - dot_v3(u, u)) * v.y + 2.0f * s * cross(u, v).y;
  result.z = 2.0f * dot_v3(u, v) * u.z + (s * s - dot_v3(u, u)) * v.z + 2.0f * s * cross(u, v).z;

  //vprime.x += origin.x;
  //vprime.y += origin.y;
  //vprime.z += origin.z;
  
  return result;
}

struct obj {

  v3 A;
  v3 B;
  
  v3 A_normalized;
  v3 A_projection;

  f32 dot_product_result;

  Color_ projection_point_color;

  Color_ point_color;
  Color_ line_color;

  Color_ normalized_point_color;
  Color_ normalized_line_color;
};

struct scene_data {
};

extern "C" void scene_update(struct Scene *self, RayAPI *engineCTX) {
  scene_data *data = (scene_data *)self->data;
  
}

extern "C" void scene_render(struct Scene *self, RayAPI *engineCTX) {
  scene_data *data = (scene_data *)self->data;

  engineCTX->ClearBackground(Color_{0, 255, 255, 255});
    
  engineCTX->DrawText(engineCTX->GetFontDefault(), 
                      engineCTX->TextFormat("Success"), 
                      v2{100, 20}, 20, 10, Color_{0, 0, 0,255});

}

extern "C" void scene_onEnter(struct Scene *self, RayAPI *engineCTX) {

  self->data = PushStruct(self->arena, scene_data);
  scene_data *data = (scene_data *)self->data;

}

extern "C" void scene_onExit(struct Scene *self) {
  memset(self->arena->memory, 0, self->arena->Used);
}
