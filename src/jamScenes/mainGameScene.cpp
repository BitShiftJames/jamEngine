
#include "../jamLibrary/jamTypes.h"
#include "../jamLibrary/jamScene.h"

#include "../jamLibrary/RayAPI.h"

// TODO[Scene]: Timing scene or something.
#if 0
struct mainGame_data {
  world global_world;
  Texture2D TexTileSheet;
  Camera2D Camera;
  total_entities global_entities;
  f32 secondUpdate;
  b32 follow_camera;
  entity *follow_entity;

  jam_rect2 render_rectangle;
  
  Shader LightShader;
  s32 LightMapLoc;
  s32 renderRecLoc;

  Texture2D LightTexture;
  Image lightPropagation1;
  Image lightPropagation2;
  u32 LightTextureDim;

  jamColor *propagationbuffer1;
  jamColor *propagationbuffer2;
};
#endif

SceneAPI void scene_onEnter(struct Scene *self) {
  ClearAnArena(self->arena);

  #if 0 
  File_box *recovered_file_box = (File_box *)self->data;

  s32 file_data_length = 0;
  u8 *file_content_base = LoadFileData(recovered_file_box->file_path, &file_data_length); 
  u8 *file_content_pointer = file_content_base;
  mainGame_data *gameData = PushStruct(self->arena, mainGame_data);

  gameData->global_world = *(world *)file_content_pointer;
  gameData->global_world.map = 0;
  gameData->global_world.map = PushArray(self->arena, gameData->global_world.Width * gameData->global_world.Height, tile);

  file_content_pointer += sizeof(world);
  v2 spawn_location = {};
  for (u32 tileY = 0; tileY < gameData->global_world.Height; tileY++) {
    for (u32 tileX = 0; tileX < gameData->global_world.Width; tileX++) {

      gameData->global_world.map[tileY * gameData->global_world.Width + tileX] = *(tile *)file_content_pointer;
      
      if (tileX == gameData->global_world.Width / 2 && tileY == (gameData->global_world.Height / 2) - 3) {
        spawn_location = {(f32)tileX * gameData->global_world.TileSize, (f32)tileY * gameData->global_world.TileSize};
      }

      file_content_pointer += sizeof(tile);
    }
  }
  UnloadFileData(file_content_base);
  
  gameData->TexTileSheet = LoadTexture("../assets/tilesheet.png");
  SetTextureFilter(gameData->TexTileSheet, TEXTURE_FILTER_POINT);

  gameData->LightTextureDim = 256;

  gameData->propagationbuffer1 = PushArray(self->arena, gameData->LightTextureDim * gameData->LightTextureDim, jamColor);
  gameData->lightPropagation1.data = gameData->propagationbuffer1;
  gameData->lightPropagation1.width = gameData->LightTextureDim;
  gameData->lightPropagation1.height = gameData->LightTextureDim;
  gameData->lightPropagation1.mipmaps = 1;
  gameData->lightPropagation1.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

  gameData->propagationbuffer2 = PushArray(self->arena, gameData->LightTextureDim * gameData->LightTextureDim, jamColor);
  gameData->lightPropagation2.data = gameData->propagationbuffer2;
  gameData->lightPropagation2.width = gameData->LightTextureDim;
  gameData->lightPropagation2.height = gameData->LightTextureDim;
  gameData->lightPropagation2.mipmaps = 1;
  gameData->lightPropagation2.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

  gameData->LightTexture = LoadTextureFromImage(gameData->lightPropagation2);
  SetTextureFilter(gameData->LightTexture, TEXTURE_FILTER_BILINEAR);
  
  gameData->LightShader = LoadShader("../shaders/basic.vert", "../shaders/lighting.frag");
  gameData->renderRecLoc = GetShaderLocation(gameData->LightShader, "renderRec");
  gameData->LightMapLoc = GetShaderLocation(gameData->LightShader, "texture1");
  
  gameData->global_entities = {};

  u32 player_entity_count = add_entity(&gameData->global_entities, v2{24, 42}, v2{spawn_location}, IGNORE);

  AddHealthComponent(gameData->global_entities.HealthComponents, gameData->global_entities.entities[player_entity_count].EntityID, 400);
  AddFallComponent(gameData->global_entities.fallComponents, gameData->global_entities.entities[player_entity_count].EntityID);

  u32 horse_entity_count = add_entity(&gameData->global_entities, v2{60, 42}, v2{spawn_location}, IDLE);
  AddHealthComponent(gameData->global_entities.HealthComponents, gameData->global_entities.entities[horse_entity_count].EntityID, 400);

  gameData->Camera.offset = {(f32)GetScreenWidth() / 2, (f32)GetScreenHeight() / 2};
  gameData->Camera.target = JamToRayVec2(spawn_location);
  gameData->Camera.zoom = 1.0f;
  gameData->follow_entity = &gameData->global_entities.entities[player_entity_count];
  gameData->follow_camera = false;

  self->data = gameData;
  #endif

}

 SceneAPI void scene_update(struct Scene *self) {
  #if 0
  mainGame_data *gameData = (mainGame_data *)self->data;

  f32 deltaTime = GetFrameTime();
  gameData->secondUpdate += deltaTime;

  if (IsKeyDown(KEY_MINUS) && gameData->Camera.zoom > .1f) {
    gameData->Camera.zoom -= .05;
  }

  if (IsKeyDown(KEY_EQUAL) && gameData->Camera.zoom < 3.0f) {
    gameData->Camera.zoom += .05;
  }

  if (IsKeyPressed(KEY_F1)) {
    gameData->follow_camera = !gameData->follow_camera;
  }

  if (gameData->follow_camera) {
    if (IsKeyDown(KEY_W)) {
      gameData->Camera.target.y -= 5;
    }
    if (IsKeyDown(KEY_A)) {
      gameData->Camera.target.x -= 5;
    }
    if (IsKeyDown(KEY_S)) {
      gameData->Camera.target.y += 5;
    }
    if (IsKeyDown(KEY_D)) {
      gameData->Camera.target.x += 5;
    }
  } else {
    update_entity_loop(&gameData->global_entities, gameData->global_world, deltaTime, gameData->secondUpdate);
    gameData->Camera.target = {gameData->follow_entity->pos.x, 
                               gameData->follow_entity->pos.y};
  }


  v2 render_distance = {256, 256};
  v2 minimum = {floor_f32((gameData->follow_entity->pos.x / gameData->global_world.TileSize) - (render_distance.x / 2)), 
                floor_f32((gameData->follow_entity->pos.y / gameData->global_world.TileSize) - (render_distance.y / 2))};

  minimum.x = Maximum(0, minimum.x);
  minimum.y = Maximum(0, minimum.y);

  gameData->render_rectangle = JamRectMinDim(minimum, render_distance);

  f32 RenderRec[4] = {
    gameData->render_rectangle.Min.x * gameData->global_world.TileSize,
    gameData->render_rectangle.Min.y * gameData->global_world.TileSize,
    gameData->render_rectangle.Max.x * gameData->global_world.TileSize,
    gameData->render_rectangle.Max.y * gameData->global_world.TileSize,
  };

  SetShaderValue(gameData->LightShader, gameData->renderRecLoc, RenderRec, SHADER_UNIFORM_VEC4);
  #endif
}

SceneAPI void scene_render(struct Scene *self, RayAPI *engineCTX) {
  #if 0
  mainGame_data *gameData = (mainGame_data *)self->data;

  ClearBackground(SKYBLUE);

  memset(gameData->propagationbuffer2, 0, gameData->LightTextureDim * gameData->LightTextureDim * sizeof(jamColor));

  InjectLighting(gameData->propagationbuffer1, gameData->global_world, gameData->global_entities, gameData->render_rectangle, gameData->LightTextureDim);

  for (u32 iter = 0; iter < 2; iter++) {
    PropagateLighting(gameData->propagationbuffer1, gameData->propagationbuffer2, gameData->LightTextureDim);
    PropagateLighting(gameData->propagationbuffer2, gameData->propagationbuffer1, gameData->LightTextureDim);
  }
  UpdateTexture(gameData->LightTexture, gameData->propagationbuffer2);

  BeginMode2D(gameData->Camera);

    BeginShaderMode(gameData->LightShader);
    
    SetShaderValueTexture(gameData->LightShader, gameData->LightMapLoc, gameData->LightTexture);
    // Reintroduce render distance.
    for (u32 tileY = gameData->render_rectangle.Min.y; tileY < gameData->render_rectangle.Max.y; tileY++) {
        for (u32 tileX = gameData->render_rectangle.Min.x; tileX < gameData->render_rectangle.Max.x; tileX++) {
          tile CurrentTile = getTile(&gameData->global_world, tileX, tileY);
          // the tiletype of zero in logic is no tile but the tile sheet of 0 is tile 1 so 
          // subtracting one fixes that bias
          s32 ActualTileType = CurrentTile.type - 1;
          if (ActualTileType < 0) {
              continue;
          }
          // some of these can be done in u32 we do not need decimal precision. For this.
          f32 spacing = (gameData->global_world.TileSize * 3);
          f32 offset = gameData->global_world.TileSize;
          u32 max_x_tile_sheet = (gameData->TexTileSheet.width - offset) / spacing;
          f32 typeX =  (ActualTileType % (max_x_tile_sheet)) * spacing + offset;
          f32 typeY =  (floor_f32((f32)ActualTileType / max_x_tile_sheet)) * spacing + offset;
          DrawTextureRec(gameData->TexTileSheet, 
                         Rectangle{typeX, typeY, (f32)gameData->global_world.TileSize, (f32)gameData->global_world.TileSize}, 
                         Vector2{(f32)(tileX * gameData->global_world.TileSize), (f32)(tileY * gameData->global_world.TileSize)}, WHITE);
      }
    }

    render_entity_loop(&gameData->global_entities);

    EndShaderMode();


  EndMode2D();

  if (gameData->secondUpdate >= 1.0f) {
    gameData->secondUpdate = 0;
  }
  #endif
}

SceneAPI void scene_onExit(struct Scene *self) {
}

