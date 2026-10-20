/* PSY-Q 4.5 libspu, member s_stsa.o: SpuSetTransferStartAddr. The first
   split had it in psyq_misc_ai.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* far_syms: D_800B7BC8 (target reaches it %hi/%lo) */
/* far_syms: D_800B7BD8 (target reaches it %hi/%lo) */
extern u16 D_800B7BC8;              /* _spu_tsa */
extern s32 D_800B7BD8;              /* _spu_mem_mode_plus */
long _spu_FsetRXXa(long, unsigned long);

/* cc97_lib: SpuSetTransferStartAddr */
/* libspu SpuSetTransferStartAddr(addr) (S_STSA.OBJ): set where in SPU RAM the next
   transfer starts. An address outside 0x1010..0x7FFF8 returns 0. Otherwise _spu_tsa
   (D_800B7BC8) = _spu_FsetRXXa(-1, addr), the address in register units (-1: compute
   only, no register is written), and the return is that value scaled back to bytes
   (<< _spu_mem_mode_plus). The range test is written as the two bounds; psyz's
   `addr - 0x1010 > 0x7EFE8` gives the same bytes. */
/* func_800A03F8 */
unsigned long SpuSetTransferStartAddr(unsigned long addr)
{
    if (addr < 0x1010 || addr > 0x7FFF8) {
        return 0;
    }
    D_800B7BC8 = _spu_FsetRXXa(-1, addr);
    return D_800B7BC8 << D_800B7BD8;
}
INCLUDE_PAD(4);
