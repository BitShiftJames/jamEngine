
#include "jamText.h"

u32 StringLength(char *string) {
  u32 Length = 0;
  for (u32 Index = 0; string != 0 && string[Index] != '\0'; Index++) {
    Length++;
  }

  return Length;
}

void StringSet(char *string, char value) {
  for (u32 Index = 0; string != 0 && string[Index] != '\0'; Index++) {
    string[Index] = value;
  }
}

void ResetSplitBuffer(SplitBuffer *buf) {
  buf->count = 0;
  StringSet(buf->storage, '\0');
}

void StringSplit(char *text, char delimiter, SplitBuffer *buf) {

  ResetSplitBuffer(buf);
  buf->locations[0] = buf->storage;

  if (text != 0) {
    buf->count = 1;

    for (u32 Index = 0; Index < buf->max_text_buffer; Index++) {
      buf->storage[Index] = text[Index];
      if (buf->storage[Index] == '\0') {
        break;
      } else if (buf->storage[Index] == delimiter) {
        buf->storage[Index] = '\0';
        buf->locations[buf->count] = buf->storage + Index + 1;
        buf->count++;

        if (buf->count == buf->max_split_count) break;
      }

    }
  }
  
}

void StringCopy(char *dst, char *src, char delimiter) {
  for (u32 Index = 0; src[Index] != delimiter; Index++) {
    dst[Index] = src[Index];
  }
}

bool TextEqual(char *string1, char *string2, char delimiter, u32 max_string_count) {

  if (string1 == 0 || string2 == 0) {
    return false;
  }

  for (u32 Index = 0; (string1[Index] != delimiter) || (string2 [Index] != delimiter); Index++) {

    // One or the other are different lengths so it's not equal.
    if ((string1[Index] == delimiter) ^ (string2[Index] == delimiter)) {
      return false;
    }
    
    if (string1[Index] != string2[Index]) {
      return false;
    }

  }

  return true;
}
