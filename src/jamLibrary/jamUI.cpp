
#include "jamTypes.h"
#include "jamText.h"
#include "jamUI.h"
#include "RayAPI.h"

static inline bool Bounds_check(u32 *count, u32 *capacity, void *element) {
  if ((*count) < (*capacity) && element != 0) { 
    return false;
  } else {
    return true;
  }
}

void push_container(u32 *count, u32 *capacity, container *containers, 
                    v2 min, v2 max, Color_ color, RayAPI *engineCTX) {
  if (Bounds_check(count, capacity, containers)) {
    // Error handling
    return;
  }

  containers[(*count)].color = color;
  containers[(*count)].rectangle.Min = min * engineCTX->ScreenSize;
  containers[(*count)].rectangle.Max = max;

  (*count)++;
}

void push_button(u32 *count, u32 *capacity, Button *buttons, memoryArena *arena, RayAPI *engineCTX, char *text, 
                 Color_ text_color, Color_ hover_color, v2 pos, f32 fontSize, Font_ font, f32 spacing,  void (*callback)(void *userdata), void *userdata) {
  if (Bounds_check(count, capacity, buttons)) {
    // Error handling
    return;
  }

  buttons[(*count)].text = PushArray(arena, StringLength(text) + 1, char);
  StringCopy(buttons[(*count)].text, text);

  buttons[(*count)].text_color = text_color;
  buttons[(*count)].hover_color = hover_color;

  buttons[(*count)].size = fontSize;

  buttons[(*count)].dim.Min = pos * engineCTX->ScreenSize;
  buttons[(*count)].dim.Max.x = engineCTX->MeasureText(font, text, fontSize, spacing);
  buttons[(*count)].dim.Max.y = fontSize;

  buttons[(*count)].callback = callback;
  buttons[(*count)].userdata = userdata;


  (*count)++;
}

void push_text_boxes(u32 *count, u32 *capacity, Text_Box *text_boxes, memoryArena *arena, RayAPI *engineCTX,
                     char *text, Color_ color, v2 pos, f32 fontSize, Font_ font, f32 spacing) {
  if (Bounds_check(count, capacity, text_boxes)) {
    // Error handling
    return;
  }

  text_boxes[(*count)].text_color = color;
  text_boxes[(*count)].pos = pos;
  text_boxes[(*count)].fontSize = fontSize;
  text_boxes[(*count)].font = font;
  text_boxes[(*count)].spacing = spacing;

  text_boxes[(*count)].text = PushArray(arena, StringLength(text) + 1, char);
  StringCopy(text_boxes[(*count)].text, text);
  
  (*count)++;
}

void Render_container(Containers *ContainersToDraw, RayAPI *engineCTX) {
  for (u32 Index = 0; Index < ContainersToDraw->count; Index++) {
    container *currContainer = &ContainersToDraw->containers[Index];

    engineCTX->DrawRectangle(currContainer->rectangle.Min, 
                             currContainer->rectangle.Max, currContainer->color);
  }
}

void Render_buttons(Buttons *ButtonsToDraw, RayAPI *engineCTX) {
  for (u32 Index = 0; Index < ButtonsToDraw->count; Index++) {
    Button *currButton = &ButtonsToDraw->buttons[Index];
    
    engineCTX->DrawText(currButton->font, currButton->text, 
                        currButton->dim.Min, currButton->fontSize, 
                        currButton->spacing, currButton->text_color);
  }
}

void Render_text_boxes(TextBoxes *TextBoxesToDraw, RayAPI *engineCTX) {
  for (u32 Index = 0; Index < TextBoxesToDraw->count; Index++) {
    Text_Box *currTextBox = &TextBoxesToDraw->textBoxes[Index];
    
    engineCTX->DrawText(currTextBox->font, currTextBox->text, 
                        currTextBox->pos, currTextBox->fontSize,
                        currTextBox->spacing, currTextBox->text_color);
  }
}
