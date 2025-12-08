#ifndef JAM_TEXT_H
#define JAM_TEXT_H

#include "jamTypes.h"

struct SplitBuffer {
  u32 max_text_buffer;
  u32 max_split_count;
  char **locations;
  char *storage;
  int count;
};

u32 StringLength(char *string);
void StringSet(char *string, char value);
void ResetSplitBuffer(SplitBuffer *buf);
void StringSplit(char *text, char delimiter, SplitBuffer *buf);
bool TextEqual(char *string1, char *string2, char delimiter = '\0', u32 max_string_count = 1024);

#endif
