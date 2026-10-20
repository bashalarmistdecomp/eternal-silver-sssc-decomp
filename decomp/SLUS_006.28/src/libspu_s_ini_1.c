/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libspu/s_ini.c), psyz (decomp/src/libspu/s_ini.c) and
   psy-q-decomp (src/spu/s_ini.c), all MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libspu, member s_ini.o, part 1 of 2: _SpuInit. The first split
   had it in psyq_misc_u.c. The tree compiles this run of the member's
   functions with other flags than the runs beside it (config/cc_flags.txt),
   so the member is in several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: _SpuInit */
/* return_slot_fill: _SpuInit */
/* far_syms: D_800B7B3C D_800B7B40 D_800B7B44 D_800B7B48 D_800B7B4C D_800B7B64 */
/* far_syms: D_800B7B68 D_800B7B9C D_800B7BCC D_800B7C10 D_800B7C14 D_800B7C18 */
extern s32 D_800B7B3C;              /* _spu_keystat */
extern s32 D_800B7B40;              /* _spu_trans_mode */
extern s32 D_800B7B44;              /* _spu_rev_flag */
extern s32 D_800B7B48;              /* _spu_rev_reserve_wa */
extern s32 D_800B7B4C;              /* _spu_rev_offsetaddr */
extern SpuReverbAttr _spu_rev_attr;
extern volatile s32 D_800B7B64;     /* _spu_RQvoice */
extern volatile s32 D_800B7B68;     /* _spu_RQmask */
extern u16 D_800B7B6C[];            /* _spu_voice_centerNote[24] */
extern s32 D_800B7B9C;              /* _spu_env */
extern s32 D_800B7BCC;              /* _spu_transMode */
extern s32 D_800B7C10;              /* _spu_AllocBlockNum */
extern s32 D_800B7C14;              /* _spu_AllocLastNum */
extern SPU_MALLOC *D_800B7C18;      /* _spu_memList */
extern s32 D_800B8140[];            /* _spu_rev_startaddr[10] */
s32 _spu_init(s32);
void SpuStart(void);
void _spu_FsetRXX(s32, u32, s32);

/* libspu _SpuInit(bHot) (S_INI.OBJ): start libspu. ResetCallback, then _spu_init(bHot)
   resets the SPU (bHot != 0 keeps the voices). Cold only: the 24 words of
   _spu_voice_centerNote = 0xC000. SpuStart hooks the SPU's DMA interrupt, once. Then
   libspu's own state is cleared: reverb off (_spu_rev_flag, _spu_rev_reserve_wa, the
   mode, depth, delay and feedback of _spu_rev_attr), the reverb work area's start
   _spu_rev_offsetaddr = _spu_rev_startaddr[0], also written to SPU register word 0xD1;
   no allocation table (_spu_AllocBlockNum, _spu_AllocLastNum, _spu_memList); transfer by
   DMA (_spu_trans_mode, _spu_transMode); no key status, an empty register queue
   (_spu_RQmask, _spu_RQvoice) and _spu_env 0. Caller in this program: SpuInit,
   with 0. */
/* func_8009DCF8 */
void _SpuInit(s32 bHot)
{
    s32 i;

    ResetCallback();
    _spu_init(bHot);
    if (bHot == 0) {
        for (i = 0; i < 24; i++) {
            D_800B7B6C[i] = 0xC000;
        }
    }
    SpuStart();
    D_800B7B44 = 0;
    D_800B7B48 = 0;
    _spu_rev_attr.mode = 0;
    _spu_rev_attr.depth.left = 0;
    _spu_rev_attr.depth.right = 0;
    _spu_rev_attr.delay = 0;
    _spu_rev_attr.feedback = 0;
    D_800B7B4C = D_800B8140[0];
    _spu_FsetRXX(0xD1, D_800B7B4C, 0);
    D_800B7C10 = 0;
    D_800B7C14 = 0;
    D_800B7C18 = 0;
    D_800B7B40 = 0;
    D_800B7BCC = 0;
    D_800B7B3C = 0;
    D_800B7B68 = 0;
    D_800B7B64 = 0;
    D_800B7B9C = 0;
}
