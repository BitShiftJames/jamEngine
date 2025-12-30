
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

SceneAPI void scene_update(struct Scene *self, RayAPI *engineCTX) {
  scene_data *data = (scene_data *)self->data;
  
  v2 mousedelta = engineCTX->GetMouseDelta();

  v3 *A = &data->test_object.A;
  v3 *B = &data->test_object.B;

  if (engineCTX->IsKeyPressed(K_LEFT)) {
    (*A).x--;
    
    data->test_object.A_normalized = *A / sqrtf(A->x * A->x + A->y * A->y + A->z * A->z);
    data->test_object.dot_product_result = dot_v3(data->test_object.A_normalized, *B);
    data->test_object.A_projection = data->test_object.dot_product_result * data->test_object.A_normalized;
  }

  if (engineCTX->IsKeyPressed(K_RIGHT)) {
    (*A).x++;
    
    data->test_object.A_normalized = *A / sqrtf(A->x * A->x + A->y * A->y + A->z * A->z);
    data->test_object.dot_product_result = dot_v3(data->test_object.A_normalized, *B);
    data->test_object.A_projection = data->test_object.dot_product_result * data->test_object.A_normalized;
  }

  if (engineCTX->IsKeyPressed(K_UP)) {
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

SceneAPI void scene_render(struct Scene *self, RayAPI *engineCTX) {
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

SceneAPI void scene_onEnter(struct Scene *self, RayAPI *engineCTX) {
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

SceneAPI void scene_onExit(struct Scene *self) {
  memset(self->arena->memory, 0, self->arena->Used);
}
