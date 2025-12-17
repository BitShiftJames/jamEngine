
#include "jamDebug.h"

void Draw_Counter() {
  // TODO: See if there is a smart way to get the time spent waiting for a frame flip.
  // that potential doesn't include platform specific functions.
  //for (u32 i = 0; i < ArrayCount(MainDebugRecords); i++) {
   // debugRecords *Counter = MainDebugRecords + i;
    //u64 CycleCount_HitCount = AtomicExchangeU64(&Counter->Clocks_HitCount, 0);
    //u32 HitCount = (CycleCount_HitCount >> 32);
    //u32 CycleCount = (CycleCount_HitCount & 0xFFFFFFFF);
    //if (HitCount > 0) {
     // DrawText(TextFormat("%s: %u - %u H: %u ", Counter->functionName, CycleCount_HitCount, (CycleCount / HitCount), HitCount), (GetScreenWidth() - 1000), 20 * i, 20, RED);
    //} else {
     // continue;
    //}
  //}
}
