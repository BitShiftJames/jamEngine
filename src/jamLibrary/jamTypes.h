#ifndef JAMTYPES_H
#define JAMTYPES_H

// TODO: Get rid of C standard library? looked through the header and can't read how to actually set these myself.
#include <stdint.h>
#include <cstring>

#define Minimum(a, b) ((a) < (b) ? (a) : (b))
#define Maximum(a, b) ((a) > (b) ? (a) : (b))
#define ArrayCount(Array) (sizeof(Array) / sizeof(Array[0]))

#define Kilobytes(number) ((number) * 1024ull)
#define Megabytes(number) (Kilobytes(number) * 1024ull)
#define Gigabytes(number) (Megabytes(number) * 1024ull)

#define jamLIGHTDEBUB_ 0
#define jamMEMORYHOG_ 1

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef u32 b32;

typedef int8_t s8;
typedef uint16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef float f32;
typedef double f64;

struct v2 {
  union {
    struct {
      f32 x;
      f32 y;
    };
    struct {
      f32 width;
      f32 height;
    };
    struct {
      f32 u;
      f32 v;
    };
    f32 E[2];
  };
};

static inline v2 zero_vector(void) {
    return v2{0};
}

struct jam_rect2 {
  union {
    struct {
      v2 Min;
      v2 Max;
    };
    struct {
      f32 x;
      f32 y;
      f32 z;
      f32 w;
    };
    f32 E[4];
  };
};

inline bool operator==(v2 A, v2 B) {
  return A.x == B.x &&
         A.y == B.y;
}

inline bool operator!=(v2 A, v2 B) {
  return A.x != B.x &&
         A.y != B.y;
}

inline v2 operator+(v2 A, v2 B) {
  v2 Result = {};

  Result.x = A.x + B.x;
  Result.y = A.y + B.y;

  return Result;
}

inline v2 operator-(v2 A, v2 B) {
  v2 Result = {};

  Result.x = A.x - B.x;
  Result.y = A.y - B.y;

  return Result;
}

inline v2 operator-(v2 A, f32 Scaler) {
  v2 Result = {};

  Result.x = A.x - Scaler;
  Result.y = A.y - Scaler;

  return Result;
}

inline v2 operator*(v2 A, v2 B) {
  v2 Result = {};

  Result.x = A.x * B.x;
  Result.y = A.y * B.y;

  return Result;
}

inline v2& operator*=(v2& A, f32 Scalar) {
  A.x *= Scalar;
  A.y *= Scalar;

  return A;
}

inline v2& operator*=(v2& A, v2 B) {
  A.x *= B.x;
  A.y *= B.y;

  return A;
}

inline v2& operator+=(v2& a, v2 b) {
  a.x += b.x;
  a.y += b.y;

  return a;
}

inline v2& operator+=(v2& a, f32 b) {
  a.x += b;
  a.y += b;

  return a;
}

inline v2 operator*(v2 A, f32 Scalar) {
  v2 Result = {};

  Result.x = A.x * Scalar;
  Result.y = A.y * Scalar;

  return Result;
}

inline v2 operator/(v2 A, f32 Scalar) {
  v2 Result = {};

  Result.x = A.x / Scalar;
  Result.y = A.y / Scalar;

  return Result;
}

inline v2 operator*(f32 Scalar, v2 A) {
  v2 Result = {};

  Result.x = A.x * Scalar;
  Result.y = A.y * Scalar;

  return Result;
}

struct v3 {
  union {
    struct {
      f32 x;
      f32 y;
      f32 z;
    };
    struct {
      f32 r;
      f32 g;
      f32 b;
    };
    f32 E[3];
  };
};

inline v3 operator-(v3 A, v3 B) {
  v3 Result = {};

  Result.x = A.x - B.x;
  Result.y = A.y - B.y;
  Result.z = A.z - B.z;

  return Result;
}

struct v4 {
  union {
    struct {
      f32 x;
      f32 y;
      f32 z;
      f32 w;
    };
    struct {
      f32 r;
      f32 g;
      f32 b;
      f32 a;
    };
    f32 E[4];
  };
};

static inline jam_rect2 JamRectMinDim(v2 Min, v2 Dim) {
  jam_rect2 Result = {};

  Result.x = Min.x;
  Result.y = Min.y;
  Result.Max.x = Min.x + Dim.x;
  Result.Max.y = Min.y + Dim.y;

  return Result;
}

static inline jam_rect2 JamRectMinDim(v2 Min, f32 Dim) {
  jam_rect2 Result = {};

  Result.x = Min.x;
  Result.y = Min.y;
  Result.Max.x = Min.x + Dim;
  Result.Max.y = Min.y + Dim;

  return Result;
}

static inline jam_rect2 JamRectMinMax(v2 Min, v2 Max) {
  jam_rect2 Result = {};

  Result.x = Min.x;
  Result.y = Min.y;
  Result.Max.x = Max.x;
  Result.Max.y = Max.y;

  return Result;
}


struct memoryArena {
  u32 Size;
  u32 Used;

  void *memory;
};

static void *
_PushSize(memoryArena *arena, size_t size) {
  u8 *result = 0;
  if (arena->Used + size < arena->Size) {
    result = (u8 *)arena->memory + arena->Used;

    arena->Used += size;

    return (void *)result;
  }

  return (void *)result;
}

inline void ClearAnArena(memoryArena *arena) {
  memset(arena->memory, 0, arena->Used);
  arena->Used = 0;
};

#define PushSize(arena, size) _PushSize((arena), (size))
#define PushArray(arena, count, type)                                          \
  (type *)PushSize(arena, (count) * sizeof(type))
#define PushStruct(arena, type) (type *)PushSize((arena), sizeof(type))

#if 1  
#define Assert(expression)                                                     \
  if((expression)) {                                                          \
    *(int *)0 = 0;                                                             \
  }
#else
#define Assert() (*(int *)0 = 0)
#endif

#endif
