#ifndef JAM_UI_H
#define JAM_UI_H

#include "jamTypes.h"

#include "RayAPI.h"

struct Button {
  jam_rect2 dim;

  Color_ text_color;
  Color_ hover_color;

  s32 size;
  char *text;
  Font_ font;
  f32 spacing;
  f32 fontSize;

  void (*callback)(void *userdata);
  void *userdata;
};

struct File_Box {
  jam_rect2 dim;

  Color_ text_color;
  Color_ hover_color;
  Color_ deletion_color;

  s32 size;
};

struct container {
  jam_rect2 rectangle;
  Color_ color;
};

struct Text_Box {
  Color_ text_color;
  v2 pos;

  Font_ font;
  f32 spacing;
  f32 fontSize;
  char *text;
};

struct File_boxes {
  u32 count;
  u32 capacity;
  File_Box *file_boxes;
};

struct Buttons {
  u32 count;
  u32 capacity;
  Button *buttons;
};

struct TextBoxes {
  u32 count;
  u32 capacity;
  Text_Box *textBoxes;
};

struct Containers {
  u32 count;
  u32 capacity;
  container *containers;
};

void push_container(u32 *count, u32 *capacity, container *containers, v2 min, v2 max, Color_ color, RayAPI *engineCTX);
void push_button(u32 *count, u32 *capacity, Button *buttons, memoryArena *arena, RayAPI *engineCTX, char *text, 
                 Color_ text_color, Color_ hover_color, v2 pos, f32 fontSize, Font_ font, f32 spacing,  void (*callback)(void *userdata), void *userdata);
void push_text_boxes(u32 *count, u32 *capacity, Text_Box *text_boxes, memoryArena *arena, RayAPI *engineCTX,
                     char *text, Color_ color, v2 pos, f32 fontSize, Font_ font, f32 spacing);
void Render_container(Containers *ContainersToDraw, RayAPI *engineCTX);
void Render_buttons(Buttons *ButtonsToDraw, RayAPI *engineCTX);
void Render_text_boxes(TextBoxes *TextBoxesToDraw, RayAPI *engineCTX);
#endif
