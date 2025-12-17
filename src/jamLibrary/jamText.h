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

u32 StringLength(const char *string);
void StringSet(char *string, char value);
void StringCopy(char *dst, const char *src, char delimiter = '\0');
void StringSplit(const char *text, char delimiter, SplitBuffer *buf);
void ResetSplitBuffer(SplitBuffer *buf);
bool TextEqual(const char *string1, const char *string2, char delimiter = '\0', u32 max_string_count = 1024);

#endif
