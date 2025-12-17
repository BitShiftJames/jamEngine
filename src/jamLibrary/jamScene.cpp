#include "jamScene.h"

#include "RayAPI.h"
#include "jamText.h"
#include "jamTypes.h"

Scene *GetScene(SceneList *sceneList, char *name) {
  Scene *result = 0;
  for (u32 Index = 0; Index < sceneList->scene_count; Index++) {
    if (TextEqual(sceneList->scene_name[Index], name)) {
      result = &sceneList->scenes[Index];
    }
  }
  return result;
}


void InsertActiveScene(SceneList *sceneList, memoryArena *arena, Scene *sceneptr, char *name) {

  if (!sceneptr) {
    return;
  }

  if (sceneList->list) {
    ActiveScene *headNode = sceneList->list;
    ActiveScene *tailNode = sceneList->list->next;

    while (tailNode) {
      headNode = tailNode;
      tailNode = headNode->next;
    }

    tailNode = PushStruct(arena, ActiveScene);
    tailNode->scene = sceneptr;
    tailNode->arena = sceneptr->arena;

    char *name_cpy = PushArray(arena, StringLength(name) + 1, char);
    StringCopy(name_cpy, name);

    tailNode->scene_name = name_cpy;

    headNode->next = tailNode;

  } else {
    sceneList->list = PushStruct(arena, ActiveScene);
    sceneList->list->scene = sceneptr;
    sceneList->list->arena = sceneptr->arena;

    char *name_cpy = PushArray(arena, StringLength(name) + 1, char);
    StringCopy(name_cpy, name);
    sceneList->list->scene_name = name_cpy;
  }
}

void AddScene(SceneList *sceneList, char *name, memoryArena *active_scene_memory, 
              memoryArena *sceneMemory, u64 size, RayAPI *engineCTX) {
  Scene *currScene = GetScene(sceneList, name);

  // Could do a memory allocation here.
  currScene->arena = PushStruct(sceneMemory, memoryArena);
  currScene->arena->Size = size;
  currScene->arena->memory = (u8 *)sceneMemory->memory + sceneMemory->Used;
  sceneMemory->Used += currScene->arena->Size;

  InsertActiveScene(sceneList, active_scene_memory, currScene, name);

  if (currScene->onEnter) {
    currScene->onEnter(currScene, engineCTX);
  }
}

Scene load_a_scene(char *path, void **dll_handle, RayAPI *engineCTX) {
  void *dllHandle = engineCTX->LoadDLLFromPath(path);

  *dll_handle = dllHandle;

  Scene result = {};
  result.update = (sceneUpdate)engineCTX->LoadFunctionFromDLL(dllHandle, (char *)"scene_update");
  result.render = (sceneRender)engineCTX->LoadFunctionFromDLL(dllHandle, (char *)"scene_render");
  result.onEnter = (sceneOnEnter)engineCTX->LoadFunctionFromDLL(dllHandle, (char *)"scene_onEnter");
  result.onExit = (sceneOnExit)engineCTX->LoadFunctionFromDLL(dllHandle, (char *)"scene_onExit");

  return result;
}

void Unload_scenes(SceneList *sceneList, RayAPI *engineCTX) {
  for (u32 Index = 0; Index < sceneList->scene_count; Index++) {
    if (sceneList->dll_handles[Index]) {
      // Future Proofing.
      sceneList->scenes[Index].onExit  = 0;
      sceneList->scenes[Index].onEnter = 0;
      sceneList->scenes[Index].update  = 0;
      sceneList->scenes[Index].render  = 0;

      engineCTX->UnloadDLLFromPath(sceneList->dll_handles[Index]);
    }
  }
}

SceneList Construct_scene_table(memoryArena *arena, u32 max_scenes, char *scene_path, FilePathList_ *list, RayAPI *engineCTX) {

  u32 scene_count = 0;
  SceneList sceneTable = {};

  sceneTable.scenes = PushArray(arena, max_scenes, Scene);
  sceneTable.dll_handles = PushArray(arena, max_scenes, void *);
  sceneTable.scene_name = PushArray(arena, max_scenes, char *);
  sceneTable.scene_path = PushArray(arena, max_scenes, char *);
  sceneTable.scene_mod_time = PushArray(arena, max_scenes, u64);
  
  sceneTable.scene_count = list->count;

  for (u32 i = 0; i < list->count; i++) {
    if (engineCTX->IsFileExtension((list->paths[i]), ".dll") 
        && !TextEqual(engineCTX->GetFileNameWithoutExt(list->paths[i]), "raylib")) {

      s32 path_text_length = StringLength((char *)list->paths[i]) + 1;
      char *currPath = PushArray(arena, path_text_length, char);
      
      s32 scene_name_text_length = StringLength(engineCTX->GetFileNameWithoutExt(list->paths[i])) + 1;
      char *scene_name_text = PushArray(arena, scene_name_text_length, char);

      StringCopy(currPath, (char *)list->paths[i]);
      StringCopy(scene_name_text, engineCTX->GetFileNameWithoutExt(list->paths[i]));

      sceneTable.scene_name[sceneTable.scene_count] = scene_name_text;

      char *copy_library = (char *)engineCTX->GetFileNameWithoutExt(currPath);

      {
      s32 text_length = StringLength(copy_library);
      engineCTX->TextAppend(copy_library, "_temp.dll", &text_length);
      }

      char *copy_path = (char *)engineCTX->GetDirectoryPath(currPath);

      {
      s32 text_length = StringLength(copy_path);
      engineCTX->TextAppend(copy_path, "\\temp\\", &text_length);
      text_length = StringLength(copy_path);
      engineCTX->TextAppend(copy_path, copy_library, &text_length);

      char *copy_path_temp = copy_path;
      copy_path = PushArray(arena, StringLength(copy_path_temp), char);

        StringCopy(copy_path, copy_path_temp);
        if (engineCTX->FileExists(copy_path)) {
            engineCTX->FileRemove(copy_path);
        }
        engineCTX->FileCopy(currPath, copy_path);
      }

      sceneTable.scene_path[scene_count] = currPath;
      sceneTable.scene_name[scene_count] = scene_name_text;
      sceneTable.scenes[scene_count] = load_a_scene(copy_path, &sceneTable.dll_handles[scene_count], engineCTX);
      sceneTable.scene_mod_time[scene_count] = engineCTX->GetFileModTime(currPath);

      scene_count++;
    }
  }

  return sceneTable;
}
