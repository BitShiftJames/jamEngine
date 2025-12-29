#include "../jamLibrary/jamScene.h"
#include "../jamLibrary/RayAPI.h"
#include "../jamLibrary/jamMath.h"
#include "../jamLibrary/jamUI.h"

#include <cmath>
#include <cstdio>
#include <cstring>

struct obj {

  v3 A;
  v3 B;

  f32 A_diameter;
  f32 A_radius;
  f32 B_diameter;
  f32 B_radius;

  Color_ point_color;
  Color_ line_color;

  f32 time_accumulator;

  bool Inside_circle;
};

struct scene_data {
  Containers containerStorage;
  Camera3D_ camera;
  Font_ fontDefault;
  obj test_object;
};

f32 length(v3 A) {
  f32 Result = sqrt(A.x * A.x + A.y * A.y + A.z * A.z);

  return Result;
}

f32 distance(v3 A, v3 B) {
  v3 v = {A.x - B.x, A.y - B.y, A.z - B.z};
  f32 Result = length(v);

  
  return Result;
}

extern "C" __declspec(dllexport) void scene_update(struct Scene *self, RayAPI *engineCTX) {
  scene_data *data = (scene_data *)self->data;
  
  v2 mousedelta = engineCTX->GetMouseDelta();

  v3 *A = &data->test_object.A;
  if (engineCTX->IsKeyPressed(K_LEFT)) {
    (*A).x--;
    
  }

  if (engineCTX->IsKeyPressed(K_RIGHT)) {
    (*A).x++;
    
  }

  if (engineCTX->IsKeyPressed(K_UP)) {
    (*A).z--;
    
  }

  if (engineCTX->IsKeyPressed(K_DOWN)) {
    (*A).z++;
    
  }

  f32 circle_distance = data->test_object.A_radius + data->test_object.B_radius - distance(data->test_object.A, data->test_object.B);
  if (circle_distance > 0.0f) {
    data->test_object.Inside_circle = true;
    data->test_object.time_accumulator += engineCTX->GetFrameTime();
    if (data->test_object.time_accumulator > 1.0f) {
      data->test_object.time_accumulator = 0;
    }
  } else {
    data->test_object.Inside_circle = false;
    data->test_object.time_accumulator = 0;
  }
}

extern "C" __declspec(dllexport) void scene_render(struct Scene *self, RayAPI *engineCTX) {
  scene_data *data = (scene_data *)self->data;

  engineCTX->ClearBackground(Color_{0, 0, 0, 255});

  engineCTX->BeginMode3D(data->camera);
    engineCTX->DrawGrid(16, 1);

    Color_ bomb_circle = {};
    if (data->test_object.Inside_circle) {
      bomb_circle = Color_{u8((127 * sinf(data->test_object.time_accumulator)) + 120) , 0, 0, 255};
    } else {
      bomb_circle = data->test_object.point_color;
    }
    engineCTX->DrawCircle3D(data->test_object.A, data->test_object.A_radius, v3{1.0, 0, 0}, 270, data->test_object.point_color);
    engineCTX->DrawCircle3D(data->test_object.B, data->test_object.B_radius, v3{1.0, 0, 0}, 270, bomb_circle);

  engineCTX->EndMode3D();

  f32 circle_distance = data->test_object.A_radius + data->test_object.B_radius - distance(data->test_object.A, data->test_object.B);
  engineCTX->DrawText(data->fontDefault, 
                      engineCTX->TextFormat("Distance Circle: %f", 
                                            circle_distance),
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

  v3 *A = &data->test_object.A;
  v3 *B = &data->test_object.B;
  
  *A = v3{1.0f, 0.0f, 4.0f};
  *B = v3{4.0f, 0.0f, -1.0f};

  data->test_object.point_color = Color_{255, 255, 255, 255};

  data->test_object.A_diameter = 0.1f;
  data->test_object.A_radius = data->test_object.A_diameter * 2;

  data->test_object.B_diameter = 1.0f;
  data->test_object.B_radius = data->test_object.B_diameter * 2;
  
  data->fontDefault = engineCTX->GetFontDefault();

}

extern "C" __declspec(dllexport) void scene_onExit(struct Scene *self) {
  memset(self->arena->memory, 0, self->arena->Used);
}
