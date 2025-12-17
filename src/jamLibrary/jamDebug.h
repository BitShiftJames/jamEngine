#ifndef JAM_DEBUG_H
#define JAM_DEBUG_H

#include "jamTypes.h"
#include "jamIntrinsics.h"

#define SNAP_SHOT_HISTORY_COUNT 128

struct debugRecords {
  char *functionName;
  char *fileName;

  u32 LineNumber;
  u32 FM;

  u64 Clocks_HitCount;
};

struct SnapShotRecords {
  u32 Shot_Count;
  u8 AverageClockTime[SNAP_SHOT_HISTORY_COUNT];
};

// Another reason to write my own lsp ffs.

struct timed_block {
  debugRecords *Record;
  u64 StartCycles;
  u32 HitCount;

  timed_block(int Counter, char *FileName, int LineNumber, char *FunctionName, u32 HitCountInit = 1) {
    HitCount = HitCountInit;

    Record->LineNumber = LineNumber;
    Record->fileName = FileName;
    Record->functionName = FunctionName;

    StartCycles = __rdtsc();
  }

  ~timed_block() {
    u64 Delta = (__rdtsc() - StartCycles) | ((u64)HitCount << 32);
    AtomicAddU64(&Record->Clocks_HitCount, Delta);
  }

};

void Draw_Counter();

#define CONCATENATE_DETAIL(x, y) x##y
#define CONCATENATE(x, y) CONCATENATE_DETAIL(x, y)

#define TIMED_BLOCK(...) timed_block CONCATENATE(timerBlock, __LINE__)(__COUNTER__,(char *)__FILE__, __LINE__ ,(char *)__FUNCTION__, ## __VA_ARGS__)

#endif
