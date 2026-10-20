/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp (src/main/psxsdk/libcd/sys.c)
   and psyz (decomp/src/libcd/sys.c), all MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libcd, member sys.o, part 6 of 7: CdSyncCallback,
   CdReadyCallback, CdControl, CdControlF, CdControlB, CdMix, CdGetSector,
   CdGetSector2. The first split had it in psyq_cd_t.c. The tree compiles
   this run of the member's functions with other flags than the runs beside
   it (config/cc_flags.txt), so the member is in several files.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* far_syms: D_800B6358 (target reaches it %hi/%lo) */

extern CdlCB D_800B6358;            /* CD_cbsync: the user's sync callback */

/* cc97_lib: CdSyncCallback */
/* return_slot_fill: CdSyncCallback */
/* libcd CdSyncCallback(func) (SYS.OBJ): install func as the sync callback CD_cbsync
   (D_800B6358) and return the one it replaces. */
/* func_800905DC */
CdlCB CdSyncCallback(CdlCB func)
{
    CdlCB old;

    old = D_800B6358;
    D_800B6358 = func;
    return old;
}
/* far_syms: D_800B635C (target reaches it %hi/%lo) */

extern CdlCB D_800B635C;            /* CD_cbready: the user's ready callback */

/* cc97_lib: CdReadyCallback */
/* return_slot_fill: CdReadyCallback */
/* libcd CdReadyCallback(func) (SYS.OBJ): install func as the ready callback CD_cbready
   (D_800B635C) and return the one it replaces. */
/* func_800905F0 */
CdlCB CdReadyCallback(CdlCB func)
{
    CdlCB old;

    old = D_800B635C;
    D_800B635C = func;
    return old;
}
/* cc97_lib: CdControl */
/* return_slot_fill: CdControl */
/* far_syms: D_800B6358 D_800B6364 */
extern u8 D_800B6364;       /* CD_status, read here as SYS.OBJ declares it (psyz's sys.c:
                               `extern u_char CD_status;`); BIOS.OBJ's own is an int */
extern s32 D_800B62D0[];
s32 CD_cw(u8 com, u8 *param, u8 *result, s32 arg3);

static inline s32 cd_cw(u8 com, u8 *param, u8 *result, s32 arg3)
{
    CdlCB old = D_800B6358;
    s32 count;

    /* SYS.OBJ's inlined command issue, shared by the three CdControls: up to four tries
       with the user's sync callback CD_cbsync (D_800B6358) taken out. Each try: a CdlNop
       first when the lid-open bit 0x10 of CD_status (D_800B6364) is set and the command is
       not itself CdlNop; a CdlSetloc with param first for the commands D_800B62D0 marks
       (CdlPlay, CdlReadN, CdlSeekL, CdlSeekP, CdlReadS) when param is given; then, with
       the callback restored, the command itself through CD_cw (arg3 != 0 =
       do not wait for its completion). 0 on success, -1 after four failed tries. */
    for (count = 3; count != -1; count--)
    {
        D_800B6358 = 0;
        if (com != 1 && (D_800B6364 & 0x10))
        {
            CD_cw(1, 0, 0, 0);
        }
        if (param == 0 || D_800B62D0[com] == 0 || !CD_cw(2, param, result, 0))
        {
            D_800B6358 = old;
            if (!CD_cw(com, param, result, arg3))
            {
                return 0;
            }
        }
    }
    D_800B6358 = old;
    return -1;
}
/* libcd CdControl(com, param, result) (SYS.OBJ): issue CD command com with its parameter
   bytes and wait for its completion (CD_cw with arg3 = 0), copying the 8 result bytes to
   result if not null. The issue is cd_cw above: up to four tries, each with a CdlNop first
   while the lid-open status bit is set and a CdlSetloc(param) first for the play / read /
   seek commands, the user's sync callback (D_800B6358) silenced for those and restored for
   the command itself. Returns 1 on success, 0 when all four tries failed. */
/* func_80090604 */
int CdControl(u_char com, u_char *param, u_char *result)
{
    return cd_cw(com, param, result, 0) == 0;
}
/* cc97_lib: CdControlF */
/* return_slot_fill: CdControlF */

/* libcd CdControlF(com, param) (SYS.OBJ): issue CD command com with its parameter bytes and
   return without waiting for its completion (CD_cw with arg3 = 1; no result buffer).
   The issue is cd_cw: up to four tries, each with a CdlNop first while the lid-open status
   bit is set and a CdlSetloc(param) first for the play / read / seek commands. Returns 1
   when the command was written, 0 when all four tries failed. */
/* func_80090740 */
s32 CdControlF(u8 com, u8 *param)
{
    return cd_cw(com, param, 0, 1) == 0;
}
/* cc97_lib: CdControlB */
/* return_slot_fill: CdControlB */
s32 CD_sync(s32 mode, u8 *result);

/* libcd CdControlB(com, param, result) (SYS.OBJ): issue CD command com with its parameter
   bytes and block until the drive has executed it. The issue is cd_cw (up to four tries,
   each with a CdlNop first while the lid-open status bit is set and a CdlSetloc(param)
   first for the play / read / seek commands; CD_cw waits for the acknowledge). If every
   try failed it returns 0; otherwise CD_sync(0, result) waits for the command's completion
   interrupt and the return is 1 when that ended as Complete (2), 0 on DiskError or a
   timeout. */
/* func_80090874 */
s32 CdControlB(u8 com, u8 *param, u8 *result)
{
    if (cd_cw(com, param, result, 0))
    {
        return 0;
    }
    return CD_sync(0, result) == 2;
}

s32 CD_vol(CdlATV *vol);
s32 CD_getsector(u8 *madr, s32 size);
s32 CD_getsector2(u8 *madr, s32 size);

/* cc97_lib: CdMix */
/* libcd CdMix(vol) (SYS.OBJ): CD_vol(vol) (BIOS.OBJ) sets the CD
   controller's audio mixer; returns 1. */
/* func_800909C0 */
s32 CdMix(CdlATV *vol)
{
    CD_vol(vol);
    return 1;
}

/* cc97_lib: CdGetSector */
/* libcd CdGetSector(madr, size) (SYS.OBJ): read size words of the sector buffer into madr
   with CD_getsector (BIOS.OBJ: the DMA is waited for); 1 when that returned 0. */
/* func_800909E0 */
int CdGetSector(void *madr, int size)
{
    return CD_getsector(madr, size) == 0;
}

/* cc97_lib: CdGetSector2 */
/* libcd CdGetSector2(madr, size) (SYS.OBJ): the same through CD_getsector2, which starts
   the DMA and does not wait. */
/* func_80090A00 */
int CdGetSector2(void *madr, int size)
{
    return CD_getsector2(madr, size) == 0;
}
