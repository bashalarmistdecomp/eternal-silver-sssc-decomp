/* PSY-Q 4.5 libcard, member init.o: InitCARD, StartCARD, StopCARD. The
   first split had it in psyq_misc_al.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

s32 EnterCriticalSection();                                /* extern */
s32 VSync(s32);                                 /* extern */
s32 ReadInitPadFlag();                                /* extern */
s32 InitCARD2(s32);                               /* extern */
s32 _patch_card_info();                                  /* extern */
s32 _patch_card();                                  /* extern */
s32 _patch_card2();                                  /* extern */
s32 _copy_memcard_patch();                                  /* extern */

/* cc97_lib: InitCARD */
/* return_slot_fill: InitCARD */

/* libcard InitCARD(val) (INIT.OBJ; the object's 64 words are the 4.6 init.o's):
   ChangeClearPAD(0), VSync(0), then inside a critical section (left only if
   EnterCriticalSection returned 1): val becomes 0 unless the pad library is initialised
   (ReadInitPadFlag), the BIOS InitCARD2(val), and PATCH.OBJ's four patch routines
   _copy_memcard_patch, _patch_card, _patch_card2 and _patch_card_info. */
/* func_800A10B8 */
void InitCARD(long val)
{
    s32 crit;

    ChangeClearPAD(0);
    VSync(0);
    crit = EnterCriticalSection();
    if (ReadInitPadFlag() == 0) {
        val = 0;
    }
    InitCARD2(val);
    _copy_memcard_patch();
    _patch_card();
    _patch_card2();
    _patch_card_info();
    if (crit == 1) {
        ExitCriticalSection();
    }
}
/* cc97_lib: StartCARD */
/* return_slot_fill: StartCARD */
s32 StartCARD2();                                  /* extern */

/* libcard StartCARD (INIT.OBJ): the BIOS StartCARD2 and
   ChangeClearPAD(0) inside a critical section (left only if EnterCriticalSection
   returned 1). Returns 0. */
/* func_800A1148 */
long StartCARD(void)
{
    s32 crit;

    crit = EnterCriticalSection();
    StartCARD2();
    ChangeClearPAD(0);
    if (crit == 1) {
        ExitCriticalSection();
    }
    return 0;
}

/* cc97_lib: StopCARD */
s32 StopCARD2();                                  /* extern */
s32 _ExitCard();                                  /* extern */

/* libcard StopCARD (INIT.OBJ): the BIOS StopCARD2, then _ExitCard
   . Returns 0. */
/* func_800A1190 */
long StopCARD(void) {
    StopCARD2();
    _ExitCard();
    return 0;
}
