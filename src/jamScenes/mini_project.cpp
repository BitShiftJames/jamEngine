
#include "../jamLibrary/RayAPI.h"
#include "../jamLibrary/jamScene.h"
#include "../jamLibrary/jamMath.h"

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>

#define Vertex_per_face 4
#define Triangle_per_face 2
#define Points_Per_triangle 3
#define num_faces_on_cube 6

struct Cubert {
  Mesh_ cubert_mesh;

  union {
    struct {
      v3 Front[Vertex_per_face];
      v3 Back[Vertex_per_face];
      v3 Top[Vertex_per_face];
      v3 Bottom[Vertex_per_face];
      v3 Left[Vertex_per_face];
      v3 Right[Vertex_per_face];
    };
    f32 raw_verts[Vertex_per_face * num_faces_on_cube * sizeof(v3)];
  };

  union {
    struct {
       v2 Front_uv[Vertex_per_face]; 
       v2 Back_uv[Vertex_per_face]; 
       v2 Top_uv[Vertex_per_face]; 
       v2 Bottom_uv[Vertex_per_face]; 
       v2 Left_uv[Vertex_per_face]; 
       v2 Right_uv[Vertex_per_face]; 
    };
    f32 raw_UVs[Vertex_per_face * num_faces_on_cube * sizeof(v2)];
  };

  union {
    struct {
      v3 Front_normals[Vertex_per_face];
      v3 Back_normals[Vertex_per_face];
      v3 Top_normals[Vertex_per_face];
      v3 Bottom_normals[Vertex_per_face];
      v3 Left_normals[Vertex_per_face];
      v3 Right_normals[Vertex_per_face];
    };
    f32 raw_normals[Vertex_per_face * num_faces_on_cube * sizeof(v3)];
  };
  
  s16 indices[num_faces_on_cube * Triangle_per_face * Points_Per_triangle];
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

  u32 cubert_count;
  Cubert *cuberts;
  // FIXME: Draw to texture.
  // pos.x, pos.y, dimension.
  v3 drawnMap;
  bool RenderToggle;
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

// Sets the UV for a face with default values.
inline void SetUV(v2 *_00, v2 *_10, v2 *_11, v2 *_01) {
  _00[0] = {0.0f, 0.0f};
  _10[0] = {1.0f, 0.0f};
  _11[0] = {1.0f, 1.0f};
  _01[0] = {0.0f, 1.0f};
}

// Sets the normal of each vertex to the normal direction specified.
inline void SetNormals(v3 *n0, v3 *n1, v3 *n2, v3 *n3, v3 normal_direction) {
  n0[0] = normal_direction;
  n1[0] = normal_direction;
  n2[0] = normal_direction;
  n3[0] = normal_direction;
}

void Construct_Cubert(v3 min, v3 max, Cubert *cubert, RayAPI *engineCTX) {

  // FIXME:
  //  None of the UV generation is correct the UV's are not all going up.
  //  They are also not scaled.
  // FIXME:
  //  The vertex tangent is not calculated for this cube which results in
  //  garbage lighting values. To calculate the vertex tangent UV's have to be
  //  Correct and you have to calculate the up vector for UV.x.
  
  v3 *Top = cubert->Top;
  Top[0] = {min.x, max.y, min.z};
  Top[1] = {min.x, max.y, max.z};
  Top[2] = {max.x, max.y, max.z};
  Top[3] = {max.x, max.y, min.z};

  SetUV(&cubert->Top_uv[0], &cubert->Top_uv[1], &cubert->Top_uv[2], &cubert->Top_uv[3]);
  SetNormals(&cubert->Top_normals[0], &cubert->Top_normals[1], 
             &cubert->Top_normals[2], &cubert->Top_normals[3], v3{0.0f, 1.0f, 0.0f});
  
  v3 *Bottom = cubert->Bottom;
  Bottom[0] = {max.x, min.y, max.z};
  Bottom[1] = {min.x, min.y, max.z};
  Bottom[2] = {min.x, min.y, min.z};
  Bottom[3] = {max.x, min.y, min.z};
  
  SetUV(&cubert->Bottom_uv[0], &cubert->Bottom_uv[1], &cubert->Bottom_uv[2], &cubert->Bottom_uv[3]);
  SetNormals(&cubert->Bottom_normals[0], &cubert->Bottom_normals[1], 
             &cubert->Bottom_normals[2], &cubert->Bottom_normals[3], v3{0.0f, -1.0f, 0.0f});

  v3 *Left= cubert->Left;
  Left[0] = {min.x, max.y, min.z};
  Left[1] = {max.x, max.y, min.z};
  Left[2] = {max.x, min.y, min.z};
  Left[3] = {min.x, min.y, min.z};

  SetUV(&cubert->Left_uv[0], &cubert->Left_uv[1], &cubert->Left_uv[2], &cubert->Left_uv[3]);
  SetNormals(&cubert->Left_normals[0], &cubert->Left_normals[1], 
             &cubert->Left_normals[2], &cubert->Left_normals[3], v3{0.0f, 0.0f, -1.0f});

  v3 *Right = cubert->Right;
  Right[0] = {max.x, min.y, max.z};
  Right[1] = {max.x, max.y, max.z};
  Right[2] = {min.x, max.y, max.z};
  Right[3] = {min.x, min.y, max.z};

  SetUV(&cubert->Right_uv[0], &cubert->Right_uv[1], &cubert->Right_uv[2], &cubert->Right_uv[3]);
  SetNormals(&cubert->Right_normals[0], &cubert->Right_normals[1], 
             &cubert->Right_normals[2], &cubert->Right_normals[3], v3{0.0f, 0.0f, 1.0f});
  
  v3 *Front = cubert->Front;
  Front[0] = {max.x, min.y, min.z};
  Front[1] = {max.x, max.y, min.z};
  Front[2] = {max.x, max.y, max.z};
  Front[3] = {max.x, min.y, max.z};

  SetUV(&cubert->Front_uv[0], &cubert->Front_uv[1], &cubert->Front_uv[2], &cubert->Front_uv[3]);
  SetNormals(&cubert->Front_normals[0], &cubert->Front_normals[1], 
             &cubert->Front_normals[2], &cubert->Front_normals[3], v3{1.0f,  0.0f, 0.0f});

  v3 *Back = cubert->Back;
  Back[0] = {min.x, min.y, min.z};
  Back[1] = {min.x, min.y, max.z};
  Back[2] = {min.x, max.y, max.z};
  Back[3] = {min.x, max.y, min.z};

  SetUV(&cubert->Back_uv[0], &cubert->Back_uv[1], &cubert->Back_uv[2], &cubert->Back_uv[3]);
  SetNormals(&cubert->Back_normals[0], &cubert->Back_normals[1], 
             &cubert->Back_normals[2], &cubert->Back_normals[3], v3{-1.0f, 0.0f, 0.0f});

  int k = 0;
  for (int i = 0; i < 36; i += 6)
  {
      cubert->indices[i] = 4*k;
      cubert->indices[i + 1] = 4*k + 1;
      cubert->indices[i + 2] = 4*k + 2;
      cubert->indices[i + 3] = 4*k;
      cubert->indices[i + 4] = 4*k + 2;
      cubert->indices[i + 5] = 4*k + 3;

      k++;
  }
  
  Mesh_ *mesh = &cubert->cubert_mesh;

  mesh->vertexCount = Vertex_per_face * num_faces_on_cube;
  mesh->triangleCount = num_faces_on_cube * Triangle_per_face;

  mesh->vertices = cubert->raw_verts;
  mesh->texcoords = cubert->raw_UVs;
  mesh->normals = cubert->raw_normals;
  mesh->indices = cubert->indices;

  printf("Mesh | vertexCount %u | triangleCount %u | vertices %p\n", mesh->vertexCount,
         mesh->triangleCount, mesh->vertices);

  engineCTX->UploadMesh(&cubert->cubert_mesh, false);
}

void CreateCluster(v3 min, v3 max, Cubert *cubert_buffer, u32 cubert_count, RayAPI *engineCTX) {
  assert(cubert_count == 4);
  assert(cubert_buffer != 0);

  v2 safe_min = {min.x + 20.0f, min.z + 20.0f};
  v2 safe_max = {max.x - 20.0f, max.z - 20.0f};

  s32 safe_min_height = max.y / 2;
  s32 safe_max_height = max.y;

  f32 spacing = 1.5f;

  v2 new_max = {(f32)engineCTX->GetRandomValue(safe_min.x, safe_max.x), (f32)engineCTX->GetRandomValue(safe_min.y, safe_max.y)};

  f32 height = (engineCTX->GetRandomValue(safe_min_height, safe_max_height)); 
  Construct_Cubert(v3{min.x, min.y, min.z}, v3{new_max.x - spacing, height, new_max.y - spacing}, &cubert_buffer[0], engineCTX);

  height = (engineCTX->GetRandomValue(safe_min_height, safe_max_height));
  Construct_Cubert(v3{(new_max.x + spacing), min.y, min.z}, v3{max.x, height, (new_max.y - spacing)}, &cubert_buffer[1], engineCTX);

  height = (engineCTX->GetRandomValue(safe_min_height, safe_max_height));
  Construct_Cubert(v3{min.x, min.y, new_max.y + spacing}, {new_max.x - spacing, height, max.z}, &cubert_buffer[2], engineCTX);

  height = (engineCTX->GetRandomValue(safe_min_height, safe_max_height));
  Construct_Cubert({new_max.x + spacing, min.y, new_max.y + spacing}, {max.x, height, max.z}, &cubert_buffer[3], engineCTX);

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
    CreateCluster(v3{0.0f, 0.0f, 0.0f}, v3{100.0f, 40.0f, 100.0f}, data->cuberts, data->cubert_count, engineCTX);
  }
  if (engineCTX->IsKeyPressed(K_T)) {
    data->RenderToggle = !data->RenderToggle;
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

void DrawNormals(v3 *normals, v3 *points, RayAPI *engineCTX, Color_ color) {
  engineCTX->DrawRay(Ray_{points[0], normals[0]}, color);
  engineCTX->DrawRay(Ray_{points[1], normals[1]}, color);
  engineCTX->DrawRay(Ray_{points[2], normals[2]}, color);
  engineCTX->DrawRay(Ray_{points[3], normals[3]}, color);
}

void NewDebugRenderCube(Cubert *cuburt, RayAPI *engineCTX) {
  v3 *bottom = cuburt->Bottom;
   
  //engineCTX->DrawTriangle3D(bottom[0], bottom[1], bottom[2], WHITE);
  //engineCTX->DrawTriangle3D(bottom[2], bottom[3], bottom[0], WHITE);

  //DrawNormals(cuburt->Bottom_normals, bottom, engineCTX, GREEN);

  v3 *top = cuburt->Top;
  
  //engineCTX->DrawTriangle3D(top[0], top[1], top[2], WHITE);
  //engineCTX->DrawTriangle3D(top[2], top[3], top[0], WHITE);

  //DrawNormals(cuburt->Top_normals, top, engineCTX, GREEN);

  v3 *front = cuburt->Front;
  
  engineCTX->DrawTriangle3D(front[0], front[1], front[2], Color_{255, 0, 255, 255});
  engineCTX->DrawTriangle3D(front[2], front[3], front[0], Color_{255, 0, 255, 255});

  DrawNormals(cuburt->Front_normals, front, engineCTX, RED);
  
  v3 *back = cuburt->Back;
  
  //engineCTX->DrawTriangle3D(back[0], back[1], back[2], GREEN);
  //engineCTX->DrawTriangle3D(back[2], back[3], back[0], GREEN);

  //DrawNormals(cuburt->Back_normals, back, engineCTX, RED);

  v3 *left = cuburt->Left;
  
  //engineCTX->DrawTriangle3D(left[0], left[1], left[2], BLUE);
  //engineCTX->DrawTriangle3D(left[2], left[3], left[0], BLUE);

  //DrawNormals(cuburt->Left_normals, left, engineCTX, BLUE);

  v3 *right = cuburt->Right;

  //engineCTX->DrawTriangle3D(right[0], right[1], right[2], RED);
  //engineCTX->DrawTriangle3D(right[2], right[3], right[0], RED);

  //DrawNormals(cuburt->Right_normals, right, engineCTX, BLUE);
}

SceneAPI void scene_render(struct Scene *self, RayAPI *engineCTX) {
  scene_data *data = (scene_data *)self->data;

  engineCTX->ClearBackground(Color_{0, 0, 0, 255});

  engineCTX->BeginMode3D(data->camera);

    engineCTX->BeginShaderMode(data->lightShader);
    engineCTX->EndShaderMode();
    
    for (u32 cubeIndex = 0; cubeIndex < data->cubert_count; cubeIndex++) {
      if (data->RenderToggle) {
        engineCTX->DrawMesh(data->cuberts[cubeIndex].cubert_mesh, data->wall_material, data->wall_matrix);
      } else {
        NewDebugRenderCube(&data->cuberts[cubeIndex], engineCTX);
      }
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
  
  data->cubert_count = 4;
  data->cuberts = PushArray(self->arena, data->cubert_count, Cubert);

  CreateCluster(v3{0.0f, 0.0f, 0.0f}, v3{100.0f, 40.0f, 100.0f}, data->cuberts, data->cubert_count, engineCTX);

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
