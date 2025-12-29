#include "../jamLibrary/jamScene.h"
#include "../jamLibrary/RayAPI.h"
#include "../jamLibrary/jamMath.h"
#include "../jamLibrary/jamUI.h"

#include <cmath>
#include <cstdio>
#include <cstring>

struct obj {

  v3 basis_a;
  v3 basis_b;
  v3 origin;
  
  s32 point_count;
  u32 point_capacity;
  v3 *points;
  bool *WorldSpace;
  
  Color_ point_color;
  Color_ line_color;
  Color_ basis_a_color;
  Color_ basis_b_color;
  Color_ origin_color;
};

struct scene_data {
  Containers containerStorage;
  Camera3D_ camera;
  Font_ fontDefault;
  obj test_object;
};

void ToWorldSpace(v3 point, v3 origin, v3 basis_a, v3 basis_b) {
}

extern "C" __declspec(dllexport) void scene_update(struct Scene *self, RayAPI *engineCTX) {
  scene_data *data = (scene_data *)self->data;
  
  v2 mousedelta = engineCTX->GetMouseDelta();

  if (data->test_object.point_capacity) {
    if (engineCTX->IsKeyPressed(K_COMMA) && data->test_object.point_count > 0) {
      data->test_object.point_count--;
    }
    if (engineCTX->IsKeyPressed(K_PERIOD) && data->test_object.point_count < data->test_object.point_capacity) {
      data->test_object.point_count++;
    }
  }

  if (engineCTX->IsKeyPressed(K_LEFT)) {
    data->test_object.points[data->test_object.point_count].x--;
  }

  if (engineCTX->IsKeyPressed(K_RIGHT)) {
    data->test_object.points[data->test_object.point_count].x++;
  }

  if (engineCTX->IsKeyPressed(K_UP)) {
    data->test_object.points[data->test_object.point_count].z--;
  }

  if (engineCTX->IsKeyPressed(K_DOWN)) {
    data->test_object.points[data->test_object.point_count].z++;
  }
  
  if (engineCTX->IsKeyPressed(K_Q)) {
    data->test_object.WorldSpace[data->test_object.point_count] = !data->test_object.WorldSpace[data->test_object.point_count];
  }

}

extern "C" __declspec(dllexport) void scene_render(struct Scene *self, RayAPI *engineCTX) {
  scene_data *data = (scene_data *)self->data;

  engineCTX->ClearBackground(Color_{0, 0, 0, 255});

  engineCTX->BeginMode3D(data->camera);
    engineCTX->DrawGrid(16, 1);

    engineCTX->DrawLine3D(v3{0, 0, 0}, data->test_object.origin, data->test_object.line_color);
    engineCTX->DrawLine3D(data->test_object.origin, data->test_object.basis_a, data->test_object.basis_a_color);
    engineCTX->DrawLine3D(data->test_object.origin, data->test_object.basis_b, data->test_object.basis_b_color);
    engineCTX->DrawCircle3D(data->test_object.origin, .1f, v3{1.0, 0, 0}, 270, data->test_object.origin_color);
    engineCTX->DrawCircle3D(data->test_object.basis_a, .1f, v3{1.0, 0, 0}, 270, data->test_object.basis_a_color);
    engineCTX->DrawCircle3D(data->test_object.basis_b, .1f, v3{1.0, 0, 0}, 270, data->test_object.basis_b_color);
    
    for (u32 Index = 0; Index < data->test_object.point_capacity; Index++) {
      v3 currPoint = data->test_object.points[Index];
      engineCTX->DrawLine3D(v3{0, 0, 0}, data->test_object.points[Index], data->test_object.line_color);
      engineCTX->DrawLine3D(data->test_object.origin, data->test_object.points[Index], data->test_object.line_color);
      engineCTX->DrawCircle3D(currPoint, .05f, v3{1.0, 0.0, 0.0}, 270, data->test_object.point_color);
    }

  engineCTX->EndMode3D();

  engineCTX->DrawText(data->fontDefault, 
                      engineCTX->TextFormat("Editing Point: %u\nLocal Position: %f, %f\nWorld Position: %f, %f", 
                                            data->test_object.point_count),
                      v2{120, 20}, 20, 2, Color_{255,255,255,255});

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

  data->test_object.line_color = Color_{125, 125, 0, 255};
  data->test_object.point_color = Color_{125, 125, 255, 255};
  data->test_object.origin_color = Color_{255, 255, 255, 255};
  data->test_object.basis_a_color = Color_{125, 255, 125, 255};
  data->test_object.basis_b_color = Color_{255, 0, 0, 255};

  data->test_object.origin = v3{-4.0f, 0.0f, 4.0f};
  data->test_object.basis_a = v3{data->test_object.origin.x, data->test_object.origin.y, data->test_object.origin.z + 1.0f};
  data->test_object.basis_b = v3{data->test_object.origin.x + 1.0f, data->test_object.origin.y, data->test_object.origin.z};

  data->test_object.point_capacity = 2;
  data->test_object.points = PushArray(self->arena, data->test_object.point_capacity, v3);
  data->test_object.WorldSpace = PushArray(self->arena, data->test_object.point_capacity, bool);

}

extern "C" __declspec(dllexport) void scene_onExit(struct Scene *self) {
  memset(self->arena->memory, 0, self->arena->Used);
}

