/* SPDX-License-Identifier: MIT
   Parts of this file match code of psyz (decomp/src/libcd/toc.c),
   MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libcd, member toc.o, part 2 of 2: CdGetToc2. The first split
   had it in psyq_cd_n.c. The tree compiles this run of the member's
   functions with other flags than the runs beside it (config/cc_flags.txt),
   so the member is in several files.
   The build must stay byte-matching. */


#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: CdGetToc2 */
/* reorder_fill: CdGetToc2 */
/* return_slot_fill: CdGetToc2 */
/* far_syms: D_800B6360 */
extern s32 D_800B6360;
extern char D_80013254[];
extern char D_80013264[];
extern char D_80013280[];
s32 CdControlB(u8 com, u8 *param, u8 *result);
s32 printf();

/* libcd CdGetToc2 (TOC.OBJ): reads the disc's table of contents into loc[]. `session` is
   not read: the parameter byte starts as 1 and no command is sent with it. With the sync
   callback removed (CdSyncCallback; restored on every exit) it sends
   CdlGetTN (0x13) for the first and last track numbers (BCD, result[1] and result[2]),
   then CdlGetTD (0x14) with parameter 0 into loc[0], then one CdlGetTD per track (its
   number as BCD) into loc[1], loc[2], ...: minute and second from result[1] and
   result[2], sector 0. Commands go through CdControlB, which waits for the result. Returns the number of tracks, or 0 when a command fails.
   CD_debug (D_800B6360) >= 2 prints the track range and every entry, any nonzero level
   the error line. */
/* func_800900CC */
s32 CdGetToc2(s32 session, CdlLOC *loc)
{
    u8 param[4];
    u8 result[8];
    CdlCB old_cb;
    s32 track_first;
    s32 track_last;
    s32 track;
    s32 i;
    s32 ntoc;

    param[0] = 1;
    old_cb = CdSyncCallback(0);
    if (!CdControlB(0x13, 0, result)) {
        goto error;
    }
    track_first = (result[1] / 16) * 10 + (result[1] % 16);
    track_last = (result[2] / 16) * 10 + (result[2] % 16);
    if (D_800B6360 >= 2) {
        printf(D_80013254, track_first, track_last);
    }
    param[0] = 0;
    if (!CdControlB(0x14, param, result)) {
        goto error;
    }
    loc->minute = result[1];
    loc->second = result[2];
    loc->sector = 0;
    for (i = 1, track = track_first; track <= track_last; i++, track++) {
        param[0] = (track / 10) * 16 + (track % 10);
        if (!CdControlB(0x14, param, result)) {
            goto error;
        }
        loc[i].minute = result[1];
        loc[i].second = result[2];
        loc[i].sector = 0;
    }
    ntoc = i - 1;
    if (D_800B6360 >= 2) {
        for (i = 0; i <= ntoc; i++) {
            printf(D_80013264, loc[i].minute, loc[i].second);
        }
    }
    CdSyncCallback(old_cb);
    return ntoc;
error:
    if (D_800B6360) {
        printf(D_80013280);
    }
    CdSyncCallback(old_cb);
    return 0;
}

INCLUDE_PAD(8);
