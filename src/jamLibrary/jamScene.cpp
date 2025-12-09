#include "jamScene.h"

#include "RayAPI.h"
#include "raylib.h"

#include "../platform_win32.h"

Scene *GetScene(SceneList *sceneList, char *name) {
  Scene *result = 0;
  for (u32 Index = 0; Index < sceneList->scene_count; Index++) {
    if (TextEqual(sceneList->scene_name[Index], name)) {
      result = &sceneList->scenes[Index];
    }
  }
  return result;
}

void AddScene(SceneList *sceneList, char *name, memoryArena *arena) {
  if (sceneList->start) {
    ActiveScene *headNode = sceneList->start;
    ActiveScene *currNode = sceneList->start->next;

    while (currNode != 0) {
      headNode = currNode;
      currNode = currNode->next;
    }

    currNode = PushStruct(arena, ActiveScene);
    currNode->scene = GetScene(sceneList, name);

    currNode->scene->onEnter(currNode->scene);

    headNode->next = currNode;
  } else {
    sceneList->start = PushStruct(arena, ActiveScene);
    ActiveScene *headNode = sceneList->start;

    headNode->scene = GetScene(sceneList, name);
    
    // SO MANY POINTERS
    headNode->scene->onEnter(headNode->scene);

  }
}

Scene load_a_scene(char *path) {
  void *dllHandle = load_a_library(path);

  Scene result = {};
  result.update = (sceneUpdate)gimme_function(dllHandle, (char *)"scene_update");
  result.render = (sceneRender)gimme_function(dllHandle, (char *)"scene_render");
  result.onEnter = (sceneOnEnter)gimme_function(dllHandle, (char *)"scene_onEnter");
  result.onExit = (sceneOnExit)gimme_function(dllHandle, (char *)"scene_onExit");

  return result;
}

SceneList Construct_scene_table(memoryArena *arena, u32 max_scenes, char *scene_path) {

  FilePathList list = LoadDirectoryFiles(scene_path);

  u32 scene_count = 0;
  SceneList sceneTable = {};

  sceneTable.scenes = PushArray(arena, max_scenes, Scene);
  sceneTable.scene_name = PushArray(arena, max_scenes, char *);

  for (u32 i = 0; i < list.count; i++) {
    if (IsFileExtension((list.paths[i]), ".dll") 
        && !TextIsEqual(GetFileNameWithoutExt(list.paths[i]), "raylib")) {

      s32 path_text_length = TextLength(list.paths[i]) + 1;
      char *currPath = PushArray(arena, path_text_length, char);
      
      s32 scene_name_text_length = TextLength(GetFileNameWithoutExt(list.paths[i])) + 1;
      char *scene_name_text = PushArray(arena, scene_name_text_length, char);


      TextCopy(currPath, list.paths[i]);
      TextCopy(scene_name_text, GetFileNameWithoutExt(list.paths[i]));

      sceneTable.scene_name[sceneTable.scene_count] = scene_name_text;
      sceneTable.scenes[sceneTable.scene_count] = load_a_scene(currPath);

      sceneTable.scene_count++;
    }
  }

  return sceneTable;
}
