/* PSY-Q 4.5 libcd, member cdread.o, part 1 of 2: func_80092EF8,
   func_80092F2C. The first split had it in psyq_cd_aa.c. The tree compiles
   this run of the member's functions with other flags than the runs beside
   it (config/cc_flags.txt), so the member is in several files.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"

/* cc97_lib: func_80092EF8 */
/* return_slot_fill: func_80092EF8 */
/* far_syms: D_800B6730 (target reaches it %hi/%lo) */
extern volatile int D_800B6730[];   /* CDREAD.OBJ's state record (psyq-libs.md) */
typedef void (*Callback)();     /* a libetc interrupt, DMA or VSync callback (libetc_intr_1.c's) */

/* libcd CDREAD.OBJ's first function (static; no SDK version on disk names it): the sync
   callback the object installs whenever it sends CdlPause (CdReadBreak,
   cb_read func_80092F2C, cb_data func_800931A0). When the pause completes it puts the
   caller's saved sync callback back (CdSyncCallback on state word [10]) and clears the
   pause-pending flag [9] that the pause wait in CdRead / CdReadBreak / CdReadSync spins on. */
void func_80092EF8(void)
{
    CdSyncCallback((CdlCB) D_800B6730[10]);
    D_800B6730[9] = 0;
}
/* cc97_lib: func_80092F2C */
/* return_slot_fill: func_80092F2C */
/* reorder_fill: func_80092F2C */
/* far_syms: D_800B62C0 (target reaches it %hi/%lo) */
/* far_syms: D_800B62C4 (target reaches it %hi/%lo) */
extern CdlCB D_800B62C0;            /* CD_cbread: the user's read callback */
extern s32 D_800B62C4;              /* CD_read_dma_mode */
extern char D_80013724[];           /* "CdRead: sector error\n" */
void func_800931A0(void);            /* cb_data */
int func_80093270(int);             /* cd_read_retry */
s32 VSync(s32);
s32 CdDataSync(s32);
s32 CdGetSector();                  /* (madr, size) */
s32 CdGetSector2();                 /* (madr, size) */
void puts();

/* libcd cb_read (CDREAD.OBJ, static; the name is the 3.5-4.0 objects'): CdRead's ready
   callback, installed by cd_read_retry (func_80093270). It keeps the result pointer in
   state word [13]. On DataReady (intr 1) with sectors still wanted ([5] > 0): for 2048-byte
   sectors ([4] == 0x200) it first reads the 12-byte header and checks its position against
   the expected sector [8] (a mismatch prints "CdRead: sector error" and sets [5] = -1); then
   it takes the sector into the write pointer [2]. In DMA mode (CD_read_dma_mode & 1) both
   reads go through CdGetSector2, the header one bracketed by
   CdDataCallback(0) / CdDataSync(0) / CdDataCallback(cb_data), and cb_data does the
   bookkeeping; otherwise through CdGetSector and [2] advances by
   [4] words, [5]--, [8]++ here. Any other interrupt sets [5] = -1. Then [6] = VSync(-1); a
   negative count retries (cd_read_retry(1)); 1200 vblanks after the start [7] the count
   becomes -1. When the count is 0, or the time is up, it restores the caller's ready (and
   data) callbacks, installs the pause's sync callback func_80092EF8, sends CdlPause through
   CdControlF, sets the pause-pending flag [9] and calls the
   user's callback CD_cbread with CdlComplete (2) or CdlDiskError (5) and the result. */
void func_80092F2C(u8 intr, u8 *result)
{
    s32 pos[3];

    D_800B6730[13] = (s32) result;
    if (intr == 1) {
        if (D_800B6730[5] > 0) {
            if (D_800B6730[4] == 0x200) {
                if (D_800B62C4 & 1) {
                    CdDataCallback(0);
                    CdGetSector2(pos, 3);
                    CdDataSync(0);
                    CdDataCallback(func_800931A0);
                } else {
                    CdGetSector(pos, 3);
                }
                if (CdPosToInt(pos) != D_800B6730[8]) {
                    puts(D_80013724);
                    D_800B6730[5] = -1;
                }
            }
            if (D_800B62C4 & 1) {
                CdGetSector2(D_800B6730[2], D_800B6730[4]);
            } else {
                CdGetSector(D_800B6730[2], D_800B6730[4]);
                D_800B6730[2] += D_800B6730[4] * 4;
                D_800B6730[5]--;
                D_800B6730[8]++;
            }
        }
    } else {
        D_800B6730[5] = -1;
    }
    D_800B6730[6] = VSync(-1);
    if (D_800B6730[5] < 0) {
        func_80093270(1);
    }
    if (VSync(-1) > D_800B6730[7] + 1200) {
        D_800B6730[5] = -1;
    }
    if (D_800B6730[5] != 0 && VSync(-1) <= D_800B6730[7] + 1200) {
        return;
    }
    CdReadyCallback((CdlCB) D_800B6730[11]);
    if (D_800B62C4 & 1) {
        CdDataCallback((Callback) D_800B6730[12]);
    }
    CdSyncCallback((CdlCB) func_80092EF8);
    CdControlF(9, 0);
    D_800B6730[9] = 1;
    if (D_800B62C0 != 0) {
        D_800B62C0(D_800B6730[5] == 0 ? 2 : 5, result);
    }
}
