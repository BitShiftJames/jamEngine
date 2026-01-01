
#include "../jamLibrary/RayAPI.h"
#include "../jamLibrary/jamScene.h"
#include "../jamLibrary/jamMath.h"

#include <cmath>
#include <cstring>

struct construction_block {
  v3 min;
  v3 max;
};

struct scene_data {
  Font_ defaultFont;
  Camera3D_ camera;
  v3 sensitivity;
  v3 look_rotation;

  u32 count;
  u32 capacity;
  construction_block *construction_blocks;
  construction_block *selected_block;

  bool RayHit;
  bool MoveObject;
  bool ScaleObject;
  bool x_lock;
  bool y_lock;
  bool z_lock;
  Ray_ RayToFollow;

  Shader_ exampleShader;
};

v4 ZeroQuaternion() {
  return v4{0, 0, 0, 1};
};

#define PIE 3.14159
#define Tau 6.28318
#define WHITE {255, 255, 255, 255}
#define BLACK {0, 0, 0, 255}

#define RED {255, 0, 0, 255}
#define GREEN {0, 255, 0, 255}
#define BLUE {0, 0, 255, 255}

#define ZeroVector {0.0f, 0.0f, 0.0f}
#define POS_X {1.0f, 0.0f, 0.0f}
#define NEG_X {-1.0f, 0.0f, 0.0f}

#define POS_Y {0.0f, 1.0f, 0.0f}
#define NEG_Y {0.0f, -1.0f, 0.0f}

#define POS_Z {0.0f, 0.0f, 1.0f}
#define NEG_Z {0.0f, 0.0f, -1.0f}

v4 AngleAxisQuaternion(f32 AngleDegrees, v3 Axis) {
  v4 result = {};
  f32 angleRadians = AngleDegrees * (Tau / 360);

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

v4 multiplyQuaternion(v4 A, v4 B) {
  v4 result = {};

  result.x = A.w * B.x + A.x * B.w + A.y * B.z - A.z * B.y;
  result.y = A.w * B.y - A.x * B.z + A.y * B.w + A.z * B.x;
  result.z = A.w * B.z + A.x * B.y - A.y * B.x + A.z * B.z;
  result.w = A.w * B.w - A.x * B.x - A.y * B.y - A.z * B.z;

  return result;
};

v3 VectorRotate(v3 forward, v4 q) {
  v3 result = {};
  
  v3 v = forward;

  v3 u = {q.x, q.y, q.z};
  f32 s = q.w;
  
  result.x = 2.0f * dot_v3(u, v) * u.x + (s * s - dot_v3(u, u)) * v.x + 2.0f * s * cross(u, v).x;
  result.y = 2.0f * dot_v3(u, v) * u.y + (s * s - dot_v3(u, u)) * v.y + 2.0f * s * cross(u, v).y;
  result.z = 2.0f * dot_v3(u, v) * u.z + (s * s - dot_v3(u, u)) * v.z + 2.0f * s * cross(u, v).z;

  return result;
}

v3 vector3RotateByAxisAngle(v3 v, v3 axis, f32 angle) {
  // Using Euler-Rodrigues Formula
  // Ref.: https://en.wikipedia.org/w/index.php?title=Euler%E2%80%93Rodrigues_formula

  v3 result = v;

  // Vector3Normalize(axis);
  float length = sqrtf(axis.x*axis.x + axis.y*axis.y + axis.z*axis.z);
  if (length == 0.0f) length = 1.0f;
  float ilength = 1.0f/length;
  axis.x *= ilength;
  axis.y *= ilength;
  axis.z *= ilength;

  angle /= 2.0f;
  float a = sinf(angle);
  float b = axis.x*a;
  float c = axis.y*a;
  float d = axis.z*a;
  a = cosf(angle);
  v3 w = { b, c, d };

  // Vector3CrossProduct(w, v)
  v3 wv = { w.y*v.z - w.z*v.y, w.z*v.x - w.x*v.z, w.x*v.y - w.y*v.x };

  // Vector3CrossProduct(w, wv)
  v3 wwv = { w.y*wv.z - w.z*wv.y, w.z*wv.x - w.x*wv.z, w.x*wv.y - w.y*wv.x };

  // Vector3Scale(wv, 2*a)
  a *= 2;
  wv.x *= a;
  wv.y *= a;
  wv.z *= a;

  // Vector3Scale(wwv, 2)
  wwv.x *= 2;
  wwv.y *= 2;
  wwv.z *= 2;

  result.x += wv.x;
  result.y += wv.y;
  result.z += wv.z;

  result.x += wwv.x;
  result.y += wwv.y;
  result.z += wwv.z;

  return result;
}

f32 vector3angle(v3 p1, v3 p2) {

    f32 result = 0.0f;

    v3 cross = { p1.y*p2.z - p1.z*p2.y, p1.z*p2.x - p1.x*p2.z, p1.x*p2.y - p1.y*p2.x };
    f32 len = sqrtf(cross.x*cross.x + cross.y*cross.y + cross.z*cross.z);
    f32 dot = (p1.x*p2.x + p1.y*p2.y + p1.z*p2.z);
    result = atan2f(len, dot);

    return result;
}

v3 normalize(v3 p1) {
  v3 result = p1;

  f32 length = sqrtf(dot_v3(p1, p1));
  result = p1 / length;

  return result;
}

void UpdateCamera(Camera3D_ *camera, v3 look_rotation, v3 movement_delta, f32 deltaTime) {
  v3 up = POS_Y;
  v3 targetOffset = NEG_Z;

  v3 yaw = vector3RotateByAxisAngle(targetOffset, up, look_rotation.x);

  f32 maxAngleUp = vector3angle(up, yaw);
  maxAngleUp -= 0.001f;
  if (-(look_rotation.y) > maxAngleUp) {look_rotation.y = -maxAngleUp; }

  f32 maxAngleDown = vector3angle(v3{0.0f, -1.0f, 0.0f}, yaw);
  maxAngleDown *= -1.0f;
  maxAngleDown += 0.001f;
  if (-(look_rotation.y) < maxAngleDown) { look_rotation.y = -maxAngleDown; }
  
  v3 right = normalize(cross(yaw, up));

  f32 pitchAngle = -look_rotation.y;
  v3 pitch = vector3RotateByAxisAngle(yaw, right, pitchAngle);

  v3 movement_vector = vector3RotateByAxisAngle(movement_delta, up, look_rotation.x);
  camera->position += movement_vector * deltaTime;
  camera->target = camera->position + pitch;

}

inline bool GetRayHit(Ray_ Ray, v3 center, v3 dim, RayAPI *engineCTX) {
  bool result = false;

  BoundingBox_ hitBoundingBox = {center - (dim / 2), center + (dim / 2)};
  RayCollision_ collision = engineCTX->GetRayCollisionBox(Ray, hitBoundingBox);
  result = collision.hit;

  return result;
}

SceneAPI void scene_update(struct Scene *self, RayAPI *engineCTX) {
  scene_data *data = (scene_data *)self->data;

  v2 mouseDelta = engineCTX->GetMouseDelta();
  if (!(data->ScaleObject || data->MoveObject)) {
    data->look_rotation.x -= mouseDelta.x * data->sensitivity.x;
    data->look_rotation.y += mouseDelta.y * data->sensitivity.y;

    data->look_rotation.x = fmodf(data->look_rotation.x, Tau);
    data->look_rotation.y = fmodf(data->look_rotation.y, Tau);
  }

  v3 acceleration = {};
  if (engineCTX->IsKeyDown(K_W)) {
    acceleration.z--;
  }
  if (engineCTX->IsKeyDown(K_A)) {
    acceleration.x--;
  }
  if (engineCTX->IsKeyDown(K_S)) {
    acceleration.z++;
  }
  if (engineCTX->IsKeyDown(K_D)) {
    acceleration.x++;
  }
  if (engineCTX->IsKeyDown(K_SPACE)) {
    acceleration.y++;
  }
  if (engineCTX->IsKeyDown(K_LEFT_SHIFT)) {
    acceleration.y--;
  }

  if (engineCTX->IsKeyPressed(K_PERIOD)) {
    data->construction_blocks[data->count].min = {2.0f, 2.0f, 2.0f};
    data->construction_blocks[data->count].max = {4.0f, 4.0f, 4.0f};
    data->selected_block = &data->construction_blocks[data->count];
    data->count++;
  }

  acceleration *= 20;

  UpdateCamera(&data->camera, data->look_rotation, acceleration, engineCTX->GetFrameTime());

  data->RayToFollow = engineCTX->ScreenToWorldRay(engineCTX->HalfScreenSize, data->camera);
  if (data->selected_block) {
    data->RayHit = GetRayHit(data->RayToFollow, data->selected_block->min, v3{1.0f, 1.0f, 1.0f}, engineCTX);
  }

  if (data->RayHit) {
    if (engineCTX->IsMouseButtonPressed(M1)) {
      data->MoveObject = true;
    }
    if (engineCTX->IsMouseButtonPressed(M2)) {
      data->ScaleObject = true;
    }
  }

  if (engineCTX->IsKeyPressed(K_X)) {
    data->x_lock = !data->x_lock;
    data->y_lock = false;
    data->z_lock = false;
  }

  if (engineCTX->IsKeyPressed(K_Y)) {
    data->x_lock = false;
    data->y_lock = !data->y_lock;
    data->z_lock = false;
  }

  if (engineCTX->IsKeyPressed(K_Z)) {
    data->x_lock = false;
    data->y_lock = false;
    data->z_lock = !data->z_lock;
  }

  if (engineCTX->IsMouseButtonReleased(M1)) {
    data->MoveObject = false;
    data->x_lock = false;
    data->y_lock = false;
    data->z_lock = false;
  }

  if (engineCTX->IsMouseButtonReleased(M2)) {
    data->ScaleObject = false;
    data->x_lock = false;
    data->y_lock = false;
    data->z_lock = false;
  }

  if (data->MoveObject || data->ScaleObject) {
    v3 To = normalize(data->camera.target - data->camera.position);
    v3 right = cross(To, data->camera.up);
    
    v3 object_z_axis = POS_Z;
    v3 object_x_axis = POS_X;
    
    f32 z_scale = dot_v3(right, object_z_axis);
    f32 x_scale = dot_v3(right, object_x_axis);
    f32 y_scale = 1.0f; 

    if (data->x_lock) {
      x_scale = sign_f32(x_scale);
      z_scale = 0;
      y_scale = 0;
    }

    if (data->y_lock) {
      z_scale = 0;
      x_scale = 0;
    }

    if (data->z_lock) {
      z_scale = sign_f32(z_scale);
      y_scale = 0;
      x_scale = 0;
    }

    if (data->MoveObject) {
      // move along axis scaled to the camera's right axis.
      data->selected_block->min.x += ((x_scale) * (mouseDelta.x * data->sensitivity.x));
      data->selected_block->min.y -= ((y_scale) * (mouseDelta.y * data->sensitivity.y));
      data->selected_block->min.z += ((z_scale) * (mouseDelta.x * data->sensitivity.x));
    }
    if (data->ScaleObject) {

      // scale along axis scaled to the camera's right axis.
      data->selected_block->max.x += ((x_scale) * (mouseDelta.x * data->sensitivity.x));
      data->selected_block->max.y -= ((y_scale) * (mouseDelta.y * data->sensitivity.y));
      data->selected_block->max.z += ((z_scale) * (mouseDelta.x * data->sensitivity.x));
    }
  }
}

v3 surface_normal(v3 p1, v3 p2, v3 p3) {
  v3 result = {};

  v3 u = p2 - p1;
  v3 v = p3 - p1;

  result = cross(u, v);

  return result;
}

SceneAPI void scene_render(struct Scene *self, RayAPI *engineCTX) {
  scene_data *data = (scene_data *)self->data;

  engineCTX->ClearBackground(Color_{0, 0, 0, 255});

  engineCTX->BeginShaderMode(data->exampleShader);
  engineCTX->BeginMode3D(data->camera);
    if ((data->MoveObject || data->ScaleObject) && data->selected_block) {
      if (data->x_lock) {
        engineCTX->DrawRay(Ray_{data->selected_block->min, POS_X}, RED);
        engineCTX->DrawRay(Ray_{data->selected_block->min, NEG_X}, RED);
      }

      if (data->y_lock) {
        engineCTX->DrawRay(Ray_{data->selected_block->min, POS_Y}, GREEN);
        engineCTX->DrawRay(Ray_{data->selected_block->min, NEG_Y}, GREEN);
      }

      if (data->z_lock) {
        engineCTX->DrawRay(Ray_{data->selected_block->min, POS_Z}, BLUE);
        engineCTX->DrawRay(Ray_{data->selected_block->min, NEG_Z}, BLUE);
      }
    }
    for (u32 Index = 0; Index < data->count; Index++) {
      construction_block *currConstructionBlock = &data->construction_blocks[Index];
      
      if (currConstructionBlock == data->selected_block) {
        Color_ selected_box_color = WHITE;
        if (data->RayHit) {
          selected_box_color = RED;
        }
        if (data->MoveObject) {
          selected_box_color = GREEN;
        }
        if (data->ScaleObject) {
          selected_box_color = BLUE;
        }

        engineCTX->DrawWireframeCube(currConstructionBlock->min, currConstructionBlock->max, selected_box_color);
        v3 SpherePosition = v3{(currConstructionBlock->min.x),
                               (currConstructionBlock->min.y),
                               (currConstructionBlock->min.z)};
        
        engineCTX->DrawWireframeCube(SpherePosition, v3{1.0f, 1.0f, 1.0f}, WHITE);
        engineCTX->DrawSphere(SpherePosition, 0.1f, 12, 12, WHITE);
      } else {
        engineCTX->DrawCube(currConstructionBlock->min, currConstructionBlock->max, WHITE);
      }
    }

  engineCTX->EndMode3D();
  engineCTX->EndShaderMode();

  if (data->selected_block) {
    engineCTX->DrawText(data->defaultFont, 
                        engineCTX->TextFormat("Box Position: (%f, %f)", data->selected_block->min.x, data->selected_block->min.y), 
                        v2{120, 20}, 20, 2, WHITE);
  }

  engineCTX->DrawRectangle(engineCTX->HalfScreenSize, v2{5, 10}, WHITE);
  engineCTX->DrawRectangle(engineCTX->HalfScreenSize, v2{10, 5}, WHITE);
}


SceneAPI void scene_onEnter(struct Scene *self, RayAPI *engineCTX) {
  self->data = PushStruct(self->arena, scene_data);

  scene_data *data = (scene_data *)self->data;

  data->camera.position = v3{-10.0f, 0.0f, 0.0f};
  data->camera.target = {0.0f, 0.0f, 0.0f};
  data->camera.up = POS_Y;
  data->camera.fovy = 90.0f;
  data->camera.projection = 0;

  data->defaultFont = engineCTX->GetFontDefault();

  data->sensitivity.x = 0.001f;
  data->sensitivity.y = 0.001f;
  data->sensitivity.z = 0.2f;


  engineCTX->DisableCursor();

  data->capacity = 100;
  data->construction_blocks = PushArray(self->arena, data->capacity, construction_block);

  data->exampleShader = engineCTX->LoadShader("../shaders/default.vert", "../shaders/default.frag");

}

SceneAPI void scene_onExit(struct Scene *self, RayAPI *engineCTX) {
  memset(self->arena->memory, 0, self->arena->Used);
}
