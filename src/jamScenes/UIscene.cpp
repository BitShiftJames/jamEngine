
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
  Containers containerStorage;
  Camera3D_ camera;
  obj test_object;
};

extern "C" __declspec(dllexport) void scene_update(struct Scene *self, RayAPI *engineCTX) {
  scene_data *data = (scene_data *)self->data;
  
  v2 mousedelta = engineCTX->GetMouseDelta();

  v3 *A = &data->test_object.A;
  v3 *B = &data->test_object.B;

  if (engineCTX->IsKeyPressed(K_LEFT)) {
    v3 *A = &data->test_object.A;

    (*A).x--;
    
    data->test_object.A_normalized = *A / sqrtf(A->x * A->x + A->y * A->y + A->z * A->z);
    data->test_object.dot_product_result = dot_v3(data->test_object.A_normalized, *B);
    data->test_object.A_projection = data->test_object.dot_product_result * data->test_object.A_normalized;
  }

  if (engineCTX->IsKeyPressed(K_RIGHT)) {
    v3 *A = &data->test_object.A;

    (*A).x++;
    
    data->test_object.A_normalized = *A / sqrtf(A->x * A->x + A->y * A->y + A->z * A->z);
    data->test_object.dot_product_result = dot_v3(data->test_object.A_normalized, *B);
    data->test_object.A_projection = data->test_object.dot_product_result * data->test_object.A_normalized;
  }

  if (engineCTX->IsKeyPressed(K_UP)) {
    v3 *A = &data->test_object.A;

    (*A).z--;
    
    data->test_object.A_normalized = *A / sqrtf(A->x * A->x + A->y * A->y + A->z * A->z);
    data->test_object.dot_product_result = dot_v3(data->test_object.A_normalized, *B);
    data->test_object.A_projection = data->test_object.dot_product_result * data->test_object.A_normalized;
  }

  if (engineCTX->IsKeyPressed(K_DOWN)) {

    (*A).z++;
    
    data->test_object.A_normalized = *A / sqrtf(A->x * A->x + A->y * A->y + A->z * A->z);
    data->test_object.dot_product_result = dot_v3(data->test_object.A_normalized, *B);
    data->test_object.A_projection = data->test_object.dot_product_result * data->test_object.A_normalized;
  }

  if (engineCTX->IsKeyPressed(K_W)) {
    (*B).z--;

    data->test_object.dot_product_result = dot_v3(data->test_object.A_normalized, *B);
    data->test_object.A_projection = data->test_object.dot_product_result * data->test_object.A_normalized;
  }
  if (engineCTX->IsKeyPressed(K_A)) {
    (*B).x--;

    data->test_object.dot_product_result = dot_v3(data->test_object.A_normalized, *B);
    data->test_object.A_projection = data->test_object.dot_product_result * data->test_object.A_normalized;
  }
  if (engineCTX->IsKeyPressed(K_S)) {
    (*B).z++;

    data->test_object.dot_product_result = dot_v3(data->test_object.A_normalized, *B);
    data->test_object.A_projection = data->test_object.dot_product_result * data->test_object.A_normalized;
  }
  if (engineCTX->IsKeyPressed(K_D)) {
    (*B).x++;

    data->test_object.dot_product_result = dot_v3(data->test_object.A_normalized, *B);
    data->test_object.A_projection = data->test_object.dot_product_result * data->test_object.A_normalized;
  }
}

extern "C" __declspec(dllexport) void scene_render(struct Scene *self, RayAPI *engineCTX) {
  scene_data *data = (scene_data *)self->data;

  engineCTX->ClearBackground(Color_{0, 0, 0, 255});

  engineCTX->BeginMode3D(data->camera);
    // I have no clue why this math is working the way it is.
    engineCTX->DrawGrid(16, 1);
    engineCTX->DrawCircle3D(v3{0, 0, 0}, 1.0f, v3{1.0, 0, 0}, 270, Color_{255, 255, 255, 255});

    engineCTX->DrawCircle3D(data->test_object.A, .1f, v3{1.0, 0, 0}, 270, data->test_object.point_color);
    engineCTX->DrawCircle3D(data->test_object.B, .1f, v3{1.0, 0, 0}, 270, data->test_object.point_color);

    engineCTX->DrawCircle3D(data->test_object.A_normalized, .1f, v3{1.0, 0, 0}, 270, data->test_object.normalized_point_color);
    engineCTX->DrawCircle3D(data->test_object.A_projection, .1f, v3{1.0, 0, 0}, 270, data->test_object.projection_point_color);

    engineCTX->DrawLine3D(v3{0, 0, 0}, data->test_object.A, data->test_object.line_color);
    engineCTX->DrawLine3D(v3{0, 0, 0}, data->test_object.B, data->test_object.line_color);
    engineCTX->DrawLine3D(v3{0, 0, 0}, data->test_object.A_normalized, data->test_object.normalized_line_color);
    
  engineCTX->EndMode3D();
  
  engineCTX->DrawText(engineCTX->GetFontDefault(), 
                      engineCTX->TextFormat("dot product result: %f", data->test_object.dot_product_result), 
                      v2{100, 20}, 20, 10, Color_{255,255,255,255});

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

  data->camera.position = v3{0.0f, 1.0f, .1f};
  data->camera.target = v3{0.0f, 0.0f, 0.0f};
  data->camera.up = v3{0.0f, 1.0f, 0.0f};
  data->camera.fovy = 15.0f;
  data->camera.projection = 1;

  v3 *A = &data->test_object.A;
  v3 *B = &data->test_object.B;
  
  *A = v3{1.0f, 0.0f, 4.0f};
  *B = v3{4.0f, 0.0f, -1.0f};
  
  data->test_object.A_normalized = *A / sqrtf(A->x * A->x + A->y * A->y + A->z * A->z);
  data->test_object.dot_product_result = dot_v3(data->test_object.A_normalized, *B);
  data->test_object.A_projection = data->test_object.dot_product_result * data->test_object.A_normalized;

  data->test_object.normalized_line_color = Color_{255, 0, 0, 255};
  data->test_object.normalized_point_color = Color_{125, 0, 0, 255};
  data->test_object.line_color  = Color_{125, 125, 255, 255};
  data->test_object.point_color = Color_{0, 255, 0, 255};

  data->test_object.projection_point_color = Color_{255, 255, 0, 255};
}

extern "C" __declspec(dllexport) void scene_onExit(struct Scene *self) {
  memset(self->arena->memory, 0, self->arena->Used);
}
