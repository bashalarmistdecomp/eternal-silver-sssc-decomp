/* PSY-Q 4.5 libcd, member c_004.o, part 1 of 2: data_ready_callback. The
   first split had it in psyq_spu_b.c. The tree compiles this run of the
   member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"

/* cc97_lib: func_8009394C */
extern s32 D_800E70DC;
extern s32 D_800E70E4;
extern s32 D_800E70E8;

/* cc97_lib: data_ready_callback */
extern u32 *D_800E7100;
extern CdlLOC D_800E9560;
extern s32 D_800E9564;
extern void (*D_800E9570)();

/* libcd data_ready_callback (C_004.OBJ): the streaming ring's "frame complete" hook.
   CdRead2 installs it as the CD data callback when the mode has
   CdlModeStream (0x100), and StCdInterrupt calls it itself at its end
   when StEmu_Addr (D_800E70F0) and StFinalSector are both non-zero. The ring
   StRingAddr (D_800E7100) starts with one 32-byte StHEADER (eight words) per sector
   slot; the header at StRingIdx2 (D_800E70E8), the first slot of the frame just read,
   gets id = 2 (StCOMPLETE), its disc position and frame number are kept in the object's
   statics D_800E9560 / D_800E9564 for StGetBackloc, StRingIdx2 moves up
   to the write index StRingIdx1 (D_800E70E4), the user's StFunc1 (D_800E9570, the first
   callback given to StSetStream) is called if set, and StFinalSector (D_800E70DC) is
   cleared.
   MARKED SPELLING: the id is stored through a u16 view of the header (`u16 *sp`;
   `sp[0] = 2;`, as StCdInterrupt reads it), not as `hdr->id = 2` on a StHEADER pointer.
   As a struct-member store the scheduler lets the load of StRingIdx1 pass it and the two
   load / store pairs after it come out swapped (29/35). The original construct is not
   known. */
/* func_800939D8 */
void data_ready_callback(void)
{
    u16 *sp = (u16 *)(D_800E7100 + D_800E70E8 * 8);

    sp[0] = 2;
    D_800E9560 = ((StHEADER *)sp)->loc;
    D_800E9564 = ((StHEADER *)sp)->frameCount;
    D_800E70E8 = D_800E70E4;
    if (D_800E9570 != 0)
    {
        D_800E9570();
    }
    D_800E70DC = 0;
}
