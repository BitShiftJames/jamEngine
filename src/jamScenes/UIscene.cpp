
#include "../jamLibrary/jamScene.h"
#include "../jamLibrary/RayAPI.h"
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

struct obj {

  v4 quaternion;

  v3 p1;
  v3 p2;
  v3 p3;
  v3 origin;

  Color_ p1_color;
  Color_ p2_color;
  Color_ p3_color;
  Color_ origin_color;
};
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

  return result;
};

v3 VectorRotate(v3 origin, v3 forward, v4 q) {
  v3 result = {};

  v3 v = forward - origin;
  result.x = v.x*(1 - 2*q.y*q.y - 2*q.z*q.z) + v.y*(2*q.x*q.y - 2*q.w*q.z) + v.z*(2*q.x*q.z + 2*q.w*q.y);
  result.y = v.x*(2*q.x*q.y + 2*q.w*q.z) + v.y*(1 - 2*q.x*q.x - 2*q.z*q.z) + v.z*(2*q.y*q.z - 2*q.w*q.x);
  result.z = v.x*(2*q.x*q.z - 2*q.w*q.y) + v.y*(2*q.y*q.z + 2*q.w*q.x) + v.z*(1 - 2*q.x*q.x - 2*q.y*q.y);
  
  result.x += origin.x;
  result.y += origin.y;
  result.z += origin.z;

  return result;
}

struct scene_data {
  Containers containerStorage;
  Camera3D_ camera;
  obj test_object;
};

extern "C" __declspec(dllexport) void scene_update(struct Scene *self, RayAPI *engineCTX) {
  scene_data *data = (scene_data *)self->data;
  
  v2 mousedelta = engineCTX->GetMouseDelta();
  if (engineCTX->IsKeyPressedRepeat(K_X)) {
    data->test_object.quaternion = multiplyQuaternion(data->test_object.quaternion, AngleAxisQuaternion(20, v3{1, 0, 0}));
  }
  if (engineCTX->IsKeyPressedRepeat(K_Y)) {
    data->test_object.quaternion = multiplyQuaternion(data->test_object.quaternion, AngleAxisQuaternion(20, v3{0, 1, 0}));
  }
  if (engineCTX->IsKeyPressedRepeat(K_Z)) {
    data->test_object.quaternion = multiplyQuaternion(data->test_object.quaternion, AngleAxisQuaternion(20, v3{0, 0, 1}));
  }
  
  if (engineCTX->IsKeyPressed(K_R)) {
    data->test_object.quaternion = ZeroQuaternion();
  }
}

extern "C" __declspec(dllexport) void scene_render(struct Scene *self, RayAPI *engineCTX) {
  scene_data *data = (scene_data *)self->data;

  engineCTX->ClearBackground(Color_{0, 0, 0, 255});

  engineCTX->BeginMode3D(data->camera);
    engineCTX->DrawSphere(VectorRotate(data->test_object.origin, 
                                       data->test_object.p1, 
                                       data->test_object.quaternion), 
                          .05f, 5, 5, Color_{255, 255, 0, 255});
    engineCTX->DrawSphere(VectorRotate(data->test_object.origin, 
                                       data->test_object.p2, 
                                       data->test_object.quaternion), 
                          .05f, 5, 5, Color_{255, 255, 0, 255});
    engineCTX->DrawSphere(VectorRotate(data->test_object.origin, 
                                       data->test_object.p3, 
                                       data->test_object.quaternion), 
                          .05f, 5, 5, Color_{255, 255, 0, 255});
    engineCTX->DrawSphere(data->test_object.origin, .1f, 5, 5, data->test_object.origin_color);

  engineCTX->EndMode3D();

  Render_container(&data->containerStorage, engineCTX);
}

extern "C" __declspec(dllexport) void scene_onEnter(struct Scene *self, RayAPI *engineCTX) {
  printf("On Enter Logic");

  self->data = PushStruct(self->arena, scene_data);
  scene_data *data = (scene_data *)self->data;
  data->containerStorage.capacity = 16;
  data->containerStorage.containers = PushArray(self->arena, data->containerStorage.capacity, container);

  push_container(&data->containerStorage.count, &data->containerStorage.capacity, 
                 data->containerStorage.containers, v2{0.2f, 0.2f}, v2{100, 500}, Color_{0, 0, 0, 255}, engineCTX);

  data->camera.position = v3{-20.0f, 5.0f, -20.0f};
  data->camera.target = v3{1.0f, 0.0f, 0.0f};
  data->camera.up = v3{0.0f, 1.0f, 0.0f};
  data->camera.fovy = 15.0f;
  data->camera.projection = 0;

  data->test_object.origin = v3{1.0f, 0.0f, 0.0f};
  data->test_object.p1 = v3{data->test_object.origin.x + 1.0f, data->test_object.origin.y, data->test_object.origin.z};
  data->test_object.p2 = v3{data->test_object.origin.x, data->test_object.origin.y + 1.0f, data->test_object.origin.z};
  data->test_object.p3 = v3{data->test_object.origin.x, data->test_object.origin.y, data->test_object.origin.z + 1.0f};

  data->test_object.origin_color = Color_{255, 255, 255, 255};
  data->test_object.p1_color = Color_{255, 0, 0, 255};
  data->test_object.p2_color = Color_{0, 255, 0, 255};
  data->test_object.p3_color = Color_{0, 0, 255, 255};

  data->test_object.quaternion = ZeroQuaternion();

  v4 Q1 = AngleAxisQuaternion(250, v3{0, 1, 0});
  v4 Q2 = AngleAxisQuaternion(260, v3{1, 0, 0});
  v4 Q3 = multiplyQuaternion(Q1, Q2);
  v4 Q4 = InverseQuaternion(Q3);

   

}

extern "C" __declspec(dllexport) void scene_onExit(struct Scene *self) {
  memset(self->arena->memory, 0, self->arena->Used);
}
