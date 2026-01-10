
#include "../jamLibrary/RayAPI.h"
#include "../jamLibrary/jamScene.h"
#include "../jamLibrary/jamMath.h"

#include <cmath>
#include <cstdio>
#include <cstring>

struct Cubert {
  v3 min;
  v3 max;
};

struct scene_data {
  Font_ defaultFont;

  Camera3D_ camera;
  v3 sensitivity;
  v3 look_rotation;

  Shader_ lightShader;
  s32 ul_cameraPosition;
  s32 ul_cameraTarget;

  Material_ wall_material;
  u32 count;
  Texture2D_ *wall_textures;
  Mesh_ wall_mesh;
  Matrix_ wall_matrix;

  v3 *cluster_points; 
  // FIXME: Draw to texture.
  // pos.x, pos.y, dimension.
  v3 drawnMap;
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


void UpdateCamera(Camera3D_ *camera, v3 look_rotation, v3 movement_delta, f32 deltaTime,
                  Shader_ shader, s32 pos_loc, s32 target_loc, RayAPI *engineCTX) {
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
  
  f32 position[3] = {camera->position.x, camera->position.y, camera->position.z};
  f32 targetion[3] = {camera->target.x, camera->target.y, camera->target.z};
  
  //f32 position[3] = {0.0f, 0.0f, 0.0f};
  //f32 targetion[3] = {0.0f, 1.0f, 0.0f};
  engineCTX->SetShaderValue(shader, pos_loc, position, SHADER_U_VEC3);
  engineCTX->SetShaderValue(shader, target_loc, targetion, SHADER_U_VEC3);
}

inline bool GetRayHit(Ray_ Ray, v3 center, v3 dim, RayAPI *engineCTX) {
  bool result = false;

  BoundingBox_ hitBoundingBox = {center - (dim / 2), center + (dim / 2)};
  RayCollision_ collision = engineCTX->GetRayCollisionBox(Ray, hitBoundingBox);
  result = collision.hit;

  return result;
}

void CreateCluster(v3 min, v3 max, v3 *point, RayAPI *engineCTX) {
  
  v2 safe_min = {min.x + 20.0f, min.z + 20.0f};
  v2 safe_max = {max.x - 20.0f, max.z - 20.0f};

  f32 spacing = 1.5f;

  v2 new_max = {(f32)engineCTX->GetRandomValue(safe_min.x, safe_max.x), (f32)engineCTX->GetRandomValue(safe_min.y, safe_max.y)};

  point[0] = {min.x, 0.0f, min.z};
  point[1] = {new_max.x - spacing, 0.0f, new_max.y - spacing};
  
  point[2] = {new_max.x + spacing, 0.0f, min.z};
  point[3] = {max.x, 0.0f, new_max.y - spacing};

  point[4] = {min.x, 0.0f, new_max.y + spacing};
  point[5] = {new_max.x - spacing, 0.0f, max.z};

  point[6] = {new_max.x + spacing, 0.0f, new_max.y + spacing};
  point[7] = {max.x, 0.0f, max.z};

};

SceneAPI void scene_update(struct Scene *self, RayAPI *engineCTX) {
  scene_data *data = (scene_data *)self->data;

  v2 mouseDelta = engineCTX->GetMouseDelta();
  data->look_rotation.x -= mouseDelta.x * data->sensitivity.x;
  data->look_rotation.y += mouseDelta.y * data->sensitivity.y;

  data->look_rotation.x = fmodf(data->look_rotation.x, Tau);
  data->look_rotation.y = fmodf(data->look_rotation.y, Tau);

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
  
  if (engineCTX->IsKeyPressed(K_R)) {
    CreateCluster(v3{0.0f, 0.0f, 0.0f}, v3{100.0f, 40.0f, 100.0f}, data->cluster_points, engineCTX);
  }
  acceleration *= 20;

  UpdateCamera(&data->camera, data->look_rotation, acceleration, engineCTX->GetFrameTime(),
               data->lightShader, data->ul_cameraPosition, data->ul_cameraTarget, engineCTX);

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

  engineCTX->BeginMode3D(data->camera);

    engineCTX->BeginShaderMode(data->lightShader);
    engineCTX->EndShaderMode();
    
    for (u32 Index = 0; Index < 7; Index++) {
      v3 p1 = data->cluster_points[Index];
      v3 p4 = data->cluster_points[Index + 1];
      v3 p2 = {p4.x, 0.0f, p1.z};
      v3 p3 = {p1.x, 0.0f, p4.z};

      u8 grayscale = (u8)((Index * 5) + 25);
      Color_ triangle_color = {grayscale, 0, grayscale, 255};
      engineCTX->DrawTriangle3D(p4, p2, p1, triangle_color);
      engineCTX->DrawTriangle3D(p1, p3, p4, triangle_color);
    }

    if (engineCTX->IsKeyDown(K_TAB)) {
      engineCTX->DrawSphere(data->cluster_points[0], 1.0f, 16, 16, RED);
      engineCTX->DrawSphere(data->cluster_points[7], 1.0f, 16, 16, RED);
    }

    if (engineCTX->IsKeyDown(K_ONE)) {
      engineCTX->DrawSphere(data->cluster_points[0], 1.0f, 16, 16, GREEN);
      engineCTX->DrawSphere(data->cluster_points[1], 1.0f, 16, 16, GREEN);
    }

    if (engineCTX->IsKeyDown(K_TWO)) {
      engineCTX->DrawSphere(data->cluster_points[2], 1.0f, 16, 16, GREEN);
      engineCTX->DrawSphere(data->cluster_points[3], 1.0f, 16, 16, GREEN);
    }

    if (engineCTX->IsKeyDown(K_THREE)) {
      engineCTX->DrawSphere(data->cluster_points[4], 1.0f, 16, 16, GREEN);
      engineCTX->DrawSphere(data->cluster_points[5], 1.0f, 16, 16, GREEN);
    }

    if (engineCTX->IsKeyDown(K_FOUR)) {
      engineCTX->DrawSphere(data->cluster_points[6], 1.0f, 16, 16, GREEN);
      engineCTX->DrawSphere(data->cluster_points[7], 1.0f, 16, 16, GREEN);
    }

  engineCTX->EndMode3D();

  engineCTX->DrawRectangle(engineCTX->HalfScreenSize, v2{5, 10}, WHITE);
  engineCTX->DrawRectangle(engineCTX->HalfScreenSize, v2{10, 5}, WHITE);
}


SceneAPI void scene_onEnter(struct Scene *self, RayAPI *engineCTX) {
  self->data = PushStruct(self->arena, scene_data);

  scene_data *data = (scene_data *)self->data;

  data->camera.position = v3{0.0f, 10.0f, 0.0f};
  data->camera.target = {0.0f, 0.0f, 0.0f};
  data->camera.up = POS_Y;
  data->camera.fovy = 90.0f;
  data->camera.projection = 0;

  data->defaultFont = engineCTX->GetFontDefault();

  data->sensitivity.x = 0.001f;
  data->sensitivity.y = 0.001f;
  data->sensitivity.z = 0.2f;


  engineCTX->DisableCursor();

  data->lightShader = engineCTX->LoadShader("../shaders/basic_lighting.vert", "../shaders/basic_lighting.frag");
  
  s32 uniform_location_ambient = engineCTX->GetShaderLocation(data->lightShader, "ambient");
  s32 uniform_location_lightColor = engineCTX->GetShaderLocation(data->lightShader, "lightColor");
  f32 ambient_value[4] = {0.5f, 0.5f, 0.5f, 0.5f};
  f32 light_color[4] = {1.0f, 1.0f, 1.0f, 1.0f};

  engineCTX->SetShaderValue(data->lightShader, uniform_location_ambient, ambient_value, SHADER_U_VEC4);
  engineCTX->SetShaderValue(data->lightShader, uniform_location_lightColor, light_color, SHADER_U_VEC4);
  data->lightShader.locs[17] = engineCTX->GetShaderLocation(data->lightShader, "normalMap");
  data->lightShader.locs[18] = engineCTX->GetShaderLocation(data->lightShader, "roughMap");

  data->ul_cameraPosition = engineCTX->GetShaderLocation(data->lightShader, "cameraPosition");
  data->ul_cameraTarget = engineCTX->GetShaderLocation(data->lightShader, "cameraTarget");

  data->count = 3;
  data->wall_textures = PushArray(self->arena, data->count, Texture2D_);

  Texture2D_ wall_diffuse = data->wall_textures[0] = engineCTX->LoadTexture("../assets/brick_basic_color.png");
  Texture2D_ wall_roughness = data->wall_textures[1] = engineCTX->LoadTexture("../assets/brick_roughness.png");
  Texture2D_ wall_normal = data->wall_textures[2] = engineCTX->LoadTexture("../assets/brick_normal_map.png");

  data->wall_material = engineCTX->LoadMaterialsDefault();

  data->wall_material.shader = data->lightShader;

  engineCTX->SetMaterialTexture(&data->wall_material, 0, wall_diffuse);
  engineCTX->SetMaterialTexture(&data->wall_material, 3, wall_roughness);
  engineCTX->SetMaterialTexture(&data->wall_material, 2, wall_normal);

  data->wall_mesh = engineCTX->GenMeshPlane(10, 10, 1, 1);
  
  data->cluster_points = PushArray(self->arena, 8, v3);

  CreateCluster(v3{0.0f, 0.0f, 0.0f}, v3{100.0f, 40.0f, 100.0f}, data->cluster_points, engineCTX);

  data->wall_matrix = Matrix_{0};
  data->wall_matrix.m0 = 1;
  data->wall_matrix.m5 = 1;
  data->wall_matrix.m10 = 1;
  data->wall_matrix.m15 = 1;

  data->wall_matrix.m13 = 0.0f;
  
  data->drawnMap = {15.0f, 15.0f, 215.0f};
    
}

SceneAPI void scene_onExit(struct Scene *self, RayAPI *engineCTX) {
  scene_data *data = (scene_data *)self->data;
    
  memset(self->arena->memory, 0, self->arena->Used);
}
