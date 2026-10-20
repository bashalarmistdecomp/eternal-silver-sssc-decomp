/* PSY-Q 4.5 libspu, member s_sra.o: _spu_setReverbAttr. The first split had
   it in psyq_misc_af.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* libspu's register block as the library declares it (psyz's libspu_private.h, `union
   SpuUnion`): the registers by name, or as 0x100 16-bit words for the functions that are
   handed a register number. SpuRXX is libspu_spu_1.c's with the reverb registers named. */
extern SpuUnion *D_800B7BB0;
/* far_syms: D_800B7BB0 */
void _spu_setReverbAttr(SpuRevParam *entry);
static __inline__ void _spu_srmp_memcpy(char *dst, char *src, u32 size)
{
    while (size--) {
        *dst++ = *src++;
    }
}

/* far_syms: D_800B7BB0 (target reaches it %hi/%lo) */

/* cc97_lib: _spu_setReverbAttr */
/* return_slot_fill: _spu_setReverbAttr */
/* libspu _spu_setReverbAttr(entry) (S_SRA.OBJ): write the reverb registers a preset names.
   Bit i of entry->flags selects the i-th of the 32 registers at 0x1C0 (dAPF1 ... vRIN);
   flags == 0 writes them all. Written, as the image's instructions have it, as the
   list it is: one line per register, each a test of its bit and a copy. */
#define REVERB_REGISTER(bit, reg)                         \
    if (every || (selected & (1u << (bit)))) {            \
        D_800B7BB0->rxx.reg = entry->reg;                 \
    }

/* func_8009FAF8 */
void _spu_setReverbAttr(SpuRevParam *entry)
{
    u32 selected = entry->flags;
    s32 every = entry->flags == 0;

    REVERB_REGISTER(0, dAPF1)
    REVERB_REGISTER(1, dAPF2)
    REVERB_REGISTER(2, vIIR)
    REVERB_REGISTER(3, vCOMB1)
    REVERB_REGISTER(4, vCOMB2)
    REVERB_REGISTER(5, vCOMB3)
    REVERB_REGISTER(6, vCOMB4)
    REVERB_REGISTER(7, vWALL)
    REVERB_REGISTER(8, vAPF1)
    REVERB_REGISTER(9, vAPF2)
    REVERB_REGISTER(10, mLSAME)
    REVERB_REGISTER(11, mRSAME)
    REVERB_REGISTER(12, mLCOMB1)
    REVERB_REGISTER(13, mRCOMB1)
    REVERB_REGISTER(14, mLCOMB2)
    REVERB_REGISTER(15, mRCOMB2)
    REVERB_REGISTER(16, dLSAME)
    REVERB_REGISTER(17, dRSAME)
    REVERB_REGISTER(18, mLDIFF)
    REVERB_REGISTER(19, mRDIFF)
    REVERB_REGISTER(20, mLCOMB3)
    REVERB_REGISTER(21, mRCOMB3)
    REVERB_REGISTER(22, mLCOMB4)
    REVERB_REGISTER(23, mRCOMB4)
    REVERB_REGISTER(24, dLDIFF)
    REVERB_REGISTER(25, dRDIFF)
    REVERB_REGISTER(26, mLAPF1)
    REVERB_REGISTER(27, mRAPF1)
    REVERB_REGISTER(28, mLAPF2)
    REVERB_REGISTER(29, mRAPF2)
    REVERB_REGISTER(30, vLIN)
    REVERB_REGISTER(31, vRIN)
}
#undef REVERB_REGISTER
